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
	float time = 0.0f;
	float torque = 0.0f;
	float cadence = 0.0f;
	float fake_position = 0.0f;

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



		chThdSleepMilliseconds(10);
	}
}


/**
 * @brief calculate the kalman correction.
 *
 * @param   angle error signal
 * @retval  model error correction
 */
void kalman_correct(const float angle_error, float out_error[3])
{
	const float ts = hardware::Tc;
	const float tsj = ts/settings.mechanics.J;

	/// kalman filter for torque sensor
	pk[0][2] = p[0][2] - p[2][2] * tsj;

	pk[0][0] = p[0][0] + Q - p[2][0] * tsj - pk[0][2] * tsj;
	pk[1][0] = p[1][0] + p[0][0]*ts - tsj*(p[1][2] + p[0][2] * ts);
	pk[2][0] = p[2][0] - tsj*p[2][2];

	pk[0][1] = p[0][1] + ts * (p[0][0] - tsj * p[2][0]) - tsj * p[2][1];
	pk[1][1] = p[1][1] + Q + p[0][1]*ts + ts * (p[1][0] + p[0][0] * ts);
	pk[2][1] = p[2][1] + p[2][0] * ts;

	//p[0][2] precalculated
	pk[1][2] = p[1][2] + p[0][2] * ts;
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
