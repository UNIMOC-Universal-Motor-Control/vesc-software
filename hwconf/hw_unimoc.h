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


// Default macros in case there is no hardware support or no need to change them.

#define ENABLE_GATE()
#define DISABLE_GATE()

#define LED_GREEN_ON()			palSetPad(GPIOB, 0)
#define LED_GREEN_OFF()			palClearPad(GPIOB, 0)
#define LED_RED_ON()			palSetPad(GPIOB, 1)
#define LED_RED_OFF()			palClearPad(GPIOB, 1)


#define DCCAL_ON()
#define DCCAL_OFF()
#define IS_DRV_FAULT()			0


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
 *
 */

#define HW_ADC_CHANNELS			12
#define HW_ADC_INJ_CHANNELS		3
#define HW_ADC_NBR_CONV			4

// ADC Indexes
#define ADC_IND_SENS1			0
#define ADC_IND_SENS2			1
#define ADC_IND_SENS3			2
#define ADC_IND_CURR1			3
#define ADC_IND_CURR2			4
#define ADC_IND_CURR3			5
#define ADC_IND_VIN_SENS		6
#define ADC_IND_EXT				8
#define ADC_IND_EXT2			11
#define ADC_IND_TEMP_MOS		7
#define ADC_IND_TEMP_MOTOR		10
#define ADC_IND_VREFINT			9

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

// ICU Peripheral for servo decoding
#define HW_ICU_TIMER			TIM3
#define HW_ICU_TIM_CLK_EN()		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE)
#define HW_ICU_DEV				ICUD3
#define HW_ICU_CHANNEL			ICU_CHANNEL_1
#define HW_ICU_GPIO_AF			GPIO_AF_TIM3
#define HW_ICU_GPIO				GPIOB
#define HW_ICU_PIN				4

// I2C Peripheral
#define HW_I2C_DEV				I2CD2
#define HW_I2C_GPIO_AF			GPIO_AF_I2C2
#define HW_I2C_SCL_PORT			GPIOB
#define HW_I2C_SCL_PIN			6
#define HW_I2C_SDA_PORT			GPIOB
#define HW_I2C_SDA_PIN			7

// Hall/encoder pins
#define HW_HALL_ENC_GPIO1		GPIOC
#define HW_HALL_ENC_PIN1		13
#define HW_HALL_ENC_GPIO2		GPIOC
#define HW_HALL_ENC_PIN2		14
#define HW_HALL_ENC_GPIO3		GPIOC
#define HW_HALL_ENC_PIN3		15
#define HW_ENC_TIM				TIM3
#define HW_ENC_TIM_AF			GPIO_AF_TIM4
#define HW_ENC_TIM_CLK_EN()		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE)
#define HW_ENC_EXTI_PORTSRC		EXTI_PortSourceGPIOC
#define HW_ENC_EXTI_PINSRC		EXTI_PinSource8
#define HW_ENC_EXTI_CH			EXTI9_5_IRQn
#define HW_ENC_EXTI_LINE		EXTI_Line8
#define HW_ENC_EXTI_ISR_VEC		EXTI9_5_IRQHandler
#define HW_ENC_TIM_ISR_CH		TIM4_IRQn
#define HW_ENC_TIM_ISR_VEC		TIM4_IRQHandler

#define BRK_GPIO				GPIOB
#define BRK_PIN					12

// SPI pins
#define HW_SPI_DEV				SPID1
#define HW_SPI_GPIO_AF			GPIO_AF_SPI1
#define HW_SPI_PORT_NSS			GPIOA
#define HW_SPI_PIN_NSS			15
#define HW_SPI_PORT_SCK			GPIOC
#define HW_SPI_PIN_SCK			10
#define HW_SPI_PORT_MOSI		GPIOC
#define HW_SPI_PIN_MOSI			12
#define HW_SPI_PORT_MISO		GPIOC
#define HW_SPI_PIN_MISO			11

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

// Macros
#define READ_HALL1()			palReadPad(HW_HALL_ENC_GPIO1, HW_HALL_ENC_PIN1)
#define READ_HALL2()			palReadPad(HW_HALL_ENC_GPIO2, HW_HALL_ENC_PIN2)
#define READ_HALL3()			palReadPad(HW_HALL_ENC_GPIO3, HW_HALL_ENC_PIN3)

// Default setting overrides
#ifndef MCCONF_DEFAULT_MOTOR_TYPE
#define MCCONF_DEFAULT_MOTOR_TYPE		MOTOR_TYPE_FOC
#endif
#ifndef MCCONF_L_MAX_ABS_CURRENT
#define MCCONF_L_MAX_ABS_CURRENT		120.0	// The maximum absolute current above which a fault is generated
#endif
#ifndef MCCONF_FOC_SAMPLE_V0_V7
#define MCCONF_FOC_SAMPLE_V0_V7			true	// Run control loop in both v0 and v7 (requires phase shunts)
#endif

// Setting limits
#define HW_LIM_CURRENT			-100.0, 100.0
#define HW_LIM_CURRENT_IN		-100.0, 100.0
#define HW_LIM_CURRENT_ABS		0.0, 120.0
#define HW_LIM_VIN				16.0, 75.0
#define HW_LIM_ERPM				-200e3, 200e3
#define HW_LIM_DUTY_MIN			0.0, 0.1
#define HW_LIM_DUTY_MAX			0.0, 0.99
#define HW_LIM_TEMP_FET			-40.0, 110.0

// Functions

#endif /* HW_UNIMOC_H_ */
