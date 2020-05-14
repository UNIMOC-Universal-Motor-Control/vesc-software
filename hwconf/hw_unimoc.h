/*
	Copyright 2012 - 2020 Benjamin Vedder	benjamin@vedder.se
	                      Alexander Brand	tecnologic86@gmail.com

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

#ifndef HW_UNIMOC_H_
#define HW_UNIMOC_H_

#define HW_NAME		"UNIMOC"

/*
 * The hardware has 3 current sensors.
 */
#define HW_HAS_3_SHUNTS

/*
 * The current sensors are in line with the motor phases,
 * which allows sampling in V0 and V7, and some extra filtering.
 */
#define HW_HAS_PHASE_SHUNTS

#define HW_USE_BRK

/*
 * Disable blackmagic probe output on SWD port
 */
#define HAS_BLACKMAGIC				0


// Default macros in case there is no hardware support or no need to change them.

#define ENABLE_GATE()
#define DISABLE_GATE()

#define LED_GREEN_ON()			palSetPad(GPIOB, 10)
#define LED_GREEN_OFF()			palClearPad(GPIOB, 10)
#define LED_RED_ON()			palSetPad(GPIOB, 2)
#define LED_RED_OFF()			palClearPad(GPIOB, 2)


#define DCCAL_ON()
#define DCCAL_OFF()

#define IS_DRV_FAULT()			0
#define IS_DRV_FAULT_2()		IS_DRV_FAULT()

// Double samples in beginning and end for positive current measurement.
// Useful when the shunt sense traces have noise that causes offset.
#define CURR1_DOUBLE_SAMPLE		0
#define CURR2_DOUBLE_SAMPLE		0
#define CURR3_DOUBLE_SAMPLE		0

#define AUX_ON()
#define AUX_OFF()

#define PHASE_FILTER_ON()
#define PHASE_FILTER_OFF()

#define CURRENT_FILTER_ON()
#define CURRENT_FILTER_OFF()

/*
 * ADC Vector
 *
 * 0:	IN13	SENS1
 * 1:	IN1		SENS2
 * 2:	IN3		SENS3
 * 3:	IN12	CURR1
 * 4:	IN0		CURR2
 * 5:	IN2		CURR3
 * 6:	IN4		VIN
 * 7:	IN5		TEMP_MOS
 * 8:	IN10	EXT
 * 9:	IN17	VREF
 * 10:	IN6		TEMP_MOTOR
 * 11:	IN11	EXT2

 */

#define HW_ADC_CHANNELS			12
#define HW_ADC_INJ_CHANNELS		3
#define HW_ADC_NBR_CONV			4

// ADC Indexes
#define ADC_IND_SENS1			13
#define ADC_IND_SENS2			1
#define ADC_IND_SENS3			3
#define ADC_IND_CURR1			12
#define ADC_IND_CURR2			0
#define ADC_IND_CURR3			2
#define ADC_IND_VIN_SENS		4
#define ADC_IND_EXT				10
#define ADC_IND_EXT2			11
#define ADC_IND_TEMP_MOS		5
#define ADC_IND_TEMP_MOTOR		6
#define ADC_IND_VREFINT			17

// ADC macros and settings

// Component parameters (can be overridden)
#ifndef V_REG
#define V_REG					3.3
#endif
#ifndef VIN_R1
#define VIN_R1					24000.0
#endif
#ifndef VIN_R2
#define VIN_R2					1200.0
#endif
#ifndef CURRENT_AMP_GAIN
#define CURRENT_AMP_GAIN		20.0
#endif
#ifndef CURRENT_SHUNT_RES
#define CURRENT_SHUNT_RES		(0.0002/3.0)
#endif

// Input voltage
#define GET_INPUT_VOLTAGE()		((V_REG / 4095.0) * (float)ADC_Value[ADC_IND_VIN_SENS] * ((VIN_R1 + VIN_R2) / VIN_R2))

// NTC Termistors
#define NTC_RES(adc_val)		(10000.0 / ((4095.0 / (float)adc_val) - 1.0)) // temp sensor on low side
#define NTC_TEMP(adc_ind)		(1.0 / ((logf(NTC_RES(ADC_Value[adc_ind]) / 10000.0) / 3380.0) + (1.0 / 298.15)) - 273.15)

#define NTC_RES_MOTOR(adc_val)	(10000.0 / ((4095.0 / (float)adc_val) - 1.0)) // Motor temp sensor on low side
#define NTC_TEMP_MOTOR(beta)	(1.0 / ((logf(NTC_RES_MOTOR(ADC_Value[ADC_IND_TEMP_MOTOR]) / 10000.0) / beta) + (1.0 / 298.15)) - 273.15)

// Voltage on ADC channel
#define ADC_VOLTS(ch)			((float)ADC_Value[ch] / 4096.0 * V_REG)

// Double samples in beginning and end for positive current measurement.
// Useful when the shunt sense traces have noise that causes offset.
#ifndef CURR1_DOUBLE_SAMPLE
#define CURR1_DOUBLE_SAMPLE		0
#endif
#ifndef CURR2_DOUBLE_SAMPLE
#define CURR2_DOUBLE_SAMPLE		0
#endif
#ifndef CURR3_DOUBLE_SAMPLE
#define CURR3_DOUBLE_SAMPLE		0
#endif

// COMM-port ADC GPIOs
#define HW_ADC_EXT_GPIO			GPIOC
#define HW_ADC_EXT_PIN			0
#define HW_ADC_EXT2_GPIO		GPIOC
#define HW_ADC_EXT2_PIN			1

// UART Peripheral
#define HW_UART_DEV				SD1
#define HW_UART_GPIO_AF			GPIO_AF_USART1
#define HW_UART_TX_PORT			GPIOB
#define HW_UART_TX_PIN			6
#define HW_UART_RX_PORT			GPIOB
#define HW_UART_RX_PIN			7

// Sin/Cos Encoder Signals. Override if available
#ifndef ENCODER_SIN_VOLTS
#define ENCODER_SIN_VOLTS()		0.0
#endif
#ifndef ENCODER_COS_VOLTS
#define ENCODER_COS_VOLTS()		0.0
#endif

// Current ADC macros. Override them for custom current measurement functions.
#ifndef GET_CURRENT1
#ifdef INVERTED_SHUNT_POLARITY
#define GET_CURRENT1()		(4095 - ADC_Value[ADC_IND_CURR1])
#else
#define GET_CURRENT1()		ADC_Value[ADC_IND_CURR1]
#endif
#endif
#ifndef GET_CURRENT2
#ifdef INVERTED_SHUNT_POLARITY
#define GET_CURRENT2()		(4095 - ADC_Value[ADC_IND_CURR2])
#else
#define GET_CURRENT2()		ADC_Value[ADC_IND_CURR2]
#endif
#endif
#ifndef GET_CURRENT3
#ifdef INVERTED_SHUNT_POLARITY
#define GET_CURRENT3()		(4095 - ADC_Value[ADC_IND_CURR3])
#else
#define GET_CURRENT3()		ADC_Value[ADC_IND_CURR3]
#endif
#endif

#ifndef HW_MAX_CURRENT_OFFSET
#define HW_MAX_CURRENT_OFFSET 				620
#endif
#ifndef MCCONF_MAX_CURRENT_UNBALANCE
#define MCCONF_MAX_CURRENT_UNBALANCE		(FAC_CURRENT * 512)
#endif
#ifndef MCCONF_MAX_CURRENT_UNBALANCE_RATE
#define MCCONF_MAX_CURRENT_UNBALANCE_RATE	0.3
#endif

// ADC Channels
#ifndef ADC_IND_EXT3
#define ADC_IND_EXT3 			ADC_IND_EXT
#endif
#ifndef ADC_IND_EXT2
#define ADC_IND_EXT2 			ADC_IND_EXT
#endif

// Measurement macros
#define ADC_V_L1				ADC_Value[ADC_IND_SENS1]
#define ADC_V_L2				ADC_Value[ADC_IND_SENS2]
#define ADC_V_L3				ADC_Value[ADC_IND_SENS3]
#define ADC_V_ZERO				(ADC_Value[ADC_IND_VIN_SENS] / 2)

// Hall/encoder pins
#define HW_HALL_ENC_GPIO1		GPIOC
#define HW_HALL_ENC_PIN1		13
#define HW_HALL_ENC_GPIO2		GPIOC
#define HW_HALL_ENC_PIN2		14
#define HW_HALL_ENC_GPIO3		GPIOC
#define HW_HALL_ENC_PIN3		15

// Macros
#define READ_HALL1()			palReadPad(HW_HALL_ENC_GPIO1, HW_HALL_ENC_PIN1)
#define READ_HALL2()			palReadPad(HW_HALL_ENC_GPIO2, HW_HALL_ENC_PIN2)
#define READ_HALL3()			palReadPad(HW_HALL_ENC_GPIO3, HW_HALL_ENC_PIN3)


// CAN device and port (default CAN1)
#ifndef HW_CANRX_PORT
#define HW_CANRX_PORT			GPIOB
#endif
#ifndef HW_CANRX_PIN
#define HW_CANRX_PIN			8
#endif
#ifndef HW_CANTX_PORT
#define HW_CANTX_PORT			GPIOB
#endif
#ifndef HW_CANTX_PIN
#define HW_CANTX_PIN			9
#endif
#ifndef HW_CAN_GPIO_AF
#define HW_CAN_GPIO_AF			GPIO_AF_CAN1
#endif
#ifndef HW_CAN_DEV
#define HW_CAN_DEV				CAND1
#endif

#ifndef HW_EARLY_INIT
#define HW_EARLY_INIT()
#endif

// Default ID
#ifndef HW_DEFAULT_ID
#define HW_DEFAULT_ID			(APPCONF_CONTROLLER_ID >= 0 ? APPCONF_CONTROLLER_ID : hw_id_from_uuid())
#endif

#ifndef HW_LIM_CURRENT
#define HW_LIM_CURRENT			-100.0, 100.0
#endif
#ifndef HW_LIM_CURRENT_ABS
#define HW_LIM_CURRENT_ABS		0.0, 120.0
#endif

#ifndef HW_LIM_FOC_CTRL_LOOP_FREQ
#define HW_LIM_FOC_CTRL_LOOP_FREQ	3000.0, 30000.0
#endif

#ifndef HW_FOC_CURRENT_FILTER_LIM
#define HW_FOC_CURRENT_FILTER_LIM	0.05, 1.0
#endif

#ifndef COMM_USE_USB
#define COMM_USE_USB				1
#endif

#ifndef PTC_TEMP_MOTOR
#if defined(NTC_RES_MOTOR) && defined(ADC_IND_TEMP_MOTOR)
#define PTC_TEMP_MOTOR(res, con, tbase)			(((NTC_RES_MOTOR(ADC_Value[ADC_IND_TEMP_MOTOR]) - res) / res) * 100 / con + tbase)
#define PTC_TEMP_MOTOR_2(res, con, tbase)			(((NTC_RES_MOTOR(ADC_Value[ADC_IND_TEMP_MOTOR_2]) - res) / res) * 100 / con + tbase)
#else
#define PTC_TEMP_MOTOR(res, con, tbase)			0.0
#define PTC_TEMP_MOTOR_2(res, con, tbase)			0.0
#endif
#endif

// Default second motor defines
#ifndef READ_HALL1_2
#define READ_HALL1_2()			READ_HALL1()
#endif
#ifndef READ_HALL2_2
#define READ_HALL2_2()			READ_HALL2()
#endif
#ifndef READ_HALL3_2
#define READ_HALL3_2()			READ_HALL3()
#endif
#ifndef ADC_IND_TEMP_MOS_M2
#define ADC_IND_TEMP_MOS_M2		ADC_IND_TEMP_MOS
#endif
#ifndef NTC_TEMP_MOTOR_2
#define NTC_TEMP_MOTOR_2(beta)	NTC_TEMP_MOTOR(beta)
#endif
#ifndef ADC_IND_TEMP_MOTOR_2
#define ADC_IND_TEMP_MOTOR_2	ADC_IND_TEMP_MOTOR
#endif
#ifndef  MOTOR_TEMP_LPF
#define MOTOR_TEMP_LPF 			0.01
#endif
#ifndef HW_ADC_CHANNELS_EXTRA
#define HW_ADC_CHANNELS_EXTRA	0
#endif
#ifndef ADC_V_L4
#define ADC_V_L4				ADC_V_L1
#endif
#ifndef ADC_V_L5
#define ADC_V_L5				ADC_V_L2
#endif
#ifndef ADC_V_L6
#define ADC_V_L6				ADC_V_L3
#endif


// Functions
void hw_init_gpio(void);
void hw_setup_adc_channels(void);
void hw_start_i2c(void);
void hw_stop_i2c(void);
void hw_try_restore_i2c(void);
uint8_t hw_id_from_uuid(void);

#endif /* HW_UNIMOC_H_ */
