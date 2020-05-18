/*
	Copyright 2019 Benjamin Vedder	benjamin@vedder.se

	This file is part of the VESC firmware.

	The VESC firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The VESC firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    */

#include "app.h"
#include "ch.h"
#include "hal.h"

// Some useful includes
#include "mc_interface.h"
#include "utils.h"
#include "encoder.h"
#include "terminal.h"
#include "comm_can.h"
#include "hw.h"
#include "commands.h"
#include "timeout.h"

#include <math.h>
#include <string.h>
#include <stdio.h>

// Threads
static THD_FUNCTION(my_thread, arg);
static THD_WORKING_AREA(my_thread_wa, 2048);

// Private functions


// Private variables
static volatile bool stop_now = true;
static volatile bool is_running = false;
const float Ts = 0.01f;
const float J = 1e-3f;
const float TsJ = Ts/J;
// 60kgf at 3V, 0kgf at 1.5V, Nm = kgf * 9.81m/s² * Cranklength
const float crank_length = 0.175f;
const float adc_to_nm = VREG/4096.0f*60.0f*9.81f*crank_length/1.5f;
const float adc_offset = 1.5f/VREG * 4096.0f;

typedef struct {
	float torque;
	float cadence;
	float fake_position;
	float pedal_power;
	uint32_t rotations;
} state_vector;

volatile state_vector x;

// Called when the custom application is started. Start our
// threads here and set up callbacks.
void app_custom_start(void) {

	stop_now = false;
	chThdCreateStatic(my_thread_wa, sizeof(my_thread_wa),
			NORMALPRIO, my_thread, NULL);
}

// Called when the custom application is stopped. Stop our threads
// and release callbacks.
void app_custom_stop(void) {

	stop_now = true;
	while (is_running) {
		chThdSleepMilliseconds(1);
	}
}

void app_custom_configure(app_configuration *conf) {
	(void)conf;
}

static THD_FUNCTION(my_thread, arg) {
	(void)arg;

	chRegSetThreadName("App PAS-Torque");

	is_running = true;

	// experiment plot
	chThdSleepMilliseconds(8000);
	commands_init_plot("Time", "Torque");
	commands_plot_add_graph("Torque");
	commands_plot_add_graph("Cadence");


//	for(;;) {
//		commands_plot_set_graph(0);
//		commands_send_plot_points(samp, mc_interface_temp_fet_filtered());
//		commands_plot_set_graph(1);
//		commands_send_plot_points(samp, GET_INPUT_VOLTAGE());
//		samp++;
//		chThdSleepMilliseconds(10);
//	}

	for(;;) {
		// Check if it is time to stop.
		if (stop_now) {
			is_running = false;
			return;
		}

		timeout_reset(); // Reset timeout if everything is OK.

		// Run your logic here. A lot of functionality is available in mc_interface.h.
		predict(&x);





		chThdSleepMilliseconds((uint32_t)(Ts * 1000.0f));
	}
}


/**
 * @brief calculate the mechanic model to estimate cadence and pedal power.
 */
void predict(state_vector* x)
{
    // torque in Nm from Sensor
    float adc = adc_to_nm*(ADC_Value[ADC_IND_EXT] - adc_offset);

    // speed in rad/s
    x->cadence += TsJ * (adc - x->torque);

    // integrate omega for phi
    x->fake_position += x->cadence * Ts;

    // pedal power
    x->pedal_power = x->cadence * x->torque;

    // limit position to 2 * pi and count rotations
    if(x->fake_position > 2.0f*M_PI )
    {
    	x->fake_position -= 2.0f*M_PI;
    	x->rotations++;
    }
    else if(x->fake_position < 0.0)
    {
    	x->fake_position += 2.0f*M_PI;
    	x->rotations--;
    }
}

/**
 * @brief correct the mechanic model to estimate pedal power.
 *
 * @param x 	 state vector
 * @param error  state error feedback
 *
 */
void correct(state_vector *x, float out_error[3])
{
    x->cadence += error[0];
    x->fake_position += error[1];
    x->torque += error[2];
}

/**
 * @brief calculate the kalman correction.
 *
 * @param   angle error signal
 * @retval  model error correction
 */
void kalman_correct(const float angle_error, float out_error[3])
{
	/// kalman filter for torque sensor
	pk[0][2] = p[0][2] - p[2][2] * TsJ;

	pk[0][0] = p[0][0] + Q - p[2][0] * TsJ - pk[0][2] * TsJ;
	pk[1][0] = p[1][0] + p[0][0]*Ts - TsJ*(p[1][2] + p[0][2] * Ts);
	pk[2][0] = p[2][0] - TsJ*p[2][2];

	pk[0][1] = p[0][1] + Ts * (p[0][0] - TsJ * p[2][0]) - TsJ * p[2][1];
	pk[1][1] = p[1][1] + Q + p[0][1]*Ts + Ts * (p[1][0] + p[0][0] * Ts);
	pk[2][1] = p[2][1] + p[2][0] * Ts;

	//p[0][2] precalculated
	pk[1][2] = p[1][2] + p[0][2] * Ts;
	pk[2][2] = p[2][2] + Q;

	s = 1.0f / (pk[1][1] + R);
	k[0] = pk[0][1] * s;
	k[1] = pk[1][1] * s;
	k[2] = pk[2][1] * s;

	float k_1 = k[1] - 1.0f;
	p[0][0] = pk[0][0] - k[0] * pk[1][0];
	p[1][0] = - pk[1][0] * k_1;
	p[2][0] = pk[2][0] - k[2] * pk[1][0];

	p[0][1] = pk[0][1] - k[0] * pk[1][1];
	p[1][1] = - pk[1][1] * k_1;
	p[2][1] = pk[2][1] - k[2] * pk[1][1];

	p[0][2] = pk[0][2] - k[0] * pk[1][2];
	p[1][2] = - pk[1][2] * k_1;
	p[2][2] = pk[2][2] - k[2] * pk[1][2];

	out_error[0] = k[0] * angle_error;
	out_error[1] = k[1] * angle_error;
	out_error[2] = k[2] * angle_error;
}
