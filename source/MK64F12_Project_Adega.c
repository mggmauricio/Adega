/*
 * Copyright 2016-2023 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    MK64F12_Project_Adega.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"
#include <math.h>
#include <string.h>
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

/*
 *

 * @brief   Application entry point.
 */
//
float res = 10000.0;
float tref = 25.0;
volatile uint32_t adc_value = 0;
volatile uint32_t ContadorPWM = 0;
volatile uint32_t ContadorADC = 0;
volatile bool PWMon = false;
float vout = 0;
float rntc = 0, ntc_temp= 0;
uint8_t txtbuff[] = "iniciou\r\n";
char VetorASerEnviado[15];
uint8_t temporizador = 0;
float betha = 3950.0;

void PIT_1_0_IRQHANDLER(void){
	ContadorPWM++;
	ContadorADC++;
	PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
	if(ContadorPWM == 4){
		GPIO_PortClear(GPIOC, 1u<<11u);
	}
	if(ContadorPWM == 10){
		GPIO_PortSet(GPIOC, 1u<<11u);
		ContadorPWM = 0;
	}
	if(ContadorADC == 30){
		adc_value = ADC16_GetChannelConversionValue(ADC0, kADC16_ChannelMuxA);
		ContadorADC = 0;
		sprintf(VetorASerEnviado, "%d,%d,%d\n\r", temporizador, (int)(ntc_temp*100), (int)(0.4*100));
//		UART_WriteBlocking(UART0. VetorASerEnviado, sizeof(VetorASerEnviado) -1);
	}
}

float convertToTemperature(float resistance)
{
    float temperature;
    float steinhart;

    steinhart = resistance / res; // R_REF é a resistência de referência
    steinhart = log(steinhart);
    steinhart /= betha; // B_VALUE é o valor B do termistor
    steinhart += 1.0 / (tref + 273.15); // T_REF é a temperatura de referência

    temperature = 1.0 / steinhart;
    temperature -= 273.15; // Converter de Kelvin para Celsius

    return temperature;
}

int main(void) {

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

    /* Init FSL debug console. */
    BOARD_InitDebugConsole();


    /* Force the counter to be placed into memory. */
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
    	vout = adc_value * (3.3/65535);
    	rntc = (vout*res)/ (5 - vout);
    	ntc_temp = convertToTemperature(rntc);
    }
    return 0 ;
}
