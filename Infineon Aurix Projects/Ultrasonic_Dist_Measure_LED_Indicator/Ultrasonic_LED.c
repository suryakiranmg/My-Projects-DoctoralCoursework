/**********************************************************************************************************************
 * \file Ultrasonic_LED.c
 * \copyright Copyright (C) Infineon Technologies AG 2019
 *
 * Use of this file is subject to the terms of use agreed between (i) you or the company in which ordinary course of
 * business you are acting and (ii) Infineon Technologies AG or its licensees. If and as long as no such terms of use
 * are agreed, use of this file is subject to following:
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization obtaining a copy of the software and
 * accompanying documentation covered by this license (the "Software") to use, reproduce, display, distribute, execute,
 * and transmit the Software, and to prepare derivative works of the Software, and to permit third-parties to whom the
 * Software is furnished to do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including the above license grant, this restriction
 * and the following disclaimer, must be included in all copies of the Software, in whole or in part, and all
 * derivative works of the Software, unless such copies or derivative works are solely in the form of
 * machine-executable object code generated by a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *********************************************************************************************************************/

/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "IfxPort.h"
#include "Bsp.h"
#include "IfxStm.h"
#include "Ifx_Types.h"


/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
#define LED1         &MODULE_P00,0                   /* LED: Port, Pin definition  --GREEN   - green wire      */
#define LED2         &MODULE_P00,1                   /* LED: Port, Pin definition  --BLUE    - yellow wire     */
#define LED3         &MODULE_P00,2                   /* LED: Port, Pin definition  --RED     - orange wire   */
#define WAIT_TIME   500                                     /* Wait time constant in milliseconds   */
uint16 g_distance = 500;

#define TRIG_PIN        &MODULE_P00,3                  /* TRIG: Port, Pin definition  Trigger    - brown wire   */
#define ECHO_PIN        &MODULE_P00,8                  /* ECHO: Port, Pin definition  Echo       - red wire   */

// White wire - VDD_USB -- VCC
// Black wire - GND


#define WAIT_TIME_5MICRO   5*0.001                  /* Wait time constant in microseconds   */
#define WAIT_TIME_10MICRO  10*0.001                  /* Wait time constant in microseconds   */
#define WAIT_TIME_50MICRO  50*0.001


uint64 g_start_ticks, g_stop_ticks, g_total_ticks ;
float32 g_freq, g_total_time;

/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/
/* This function initializes the port pin which drives the LED */
void initLED(void)
{
    /* Initialization of the LED used in this example */
    IfxPort_setPinModeOutput(LED1, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(LED2, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(LED3, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);

    /* Switch OFF the LED (low-level active) */
    IfxPort_setPinHigh(LED1);
    IfxPort_setPinHigh(LED2);
    IfxPort_setPinHigh(LED3);
}

/* This function toggles the port pin and wait 500 milliseconds */
void blinkLED(void)
{

           if(g_distance<20) /* Turn on LED1 only */
           {
               IfxPort_setPinState(LED1, IfxPort_State_high);
               IfxPort_setPinState(LED2, IfxPort_State_low);
               IfxPort_setPinState(LED3, IfxPort_State_low);
           }
           else if(g_distance>=20 && g_distance<40) /* Turn on LED2 only */
           {
               IfxPort_setPinState(LED1, IfxPort_State_low);
               IfxPort_setPinState(LED2, IfxPort_State_high);
               IfxPort_setPinState(LED3, IfxPort_State_low);
           }
           else if(g_distance>=40 && g_distance<60) /* Turn on LED3 only */
           {
               IfxPort_setPinState(LED1, IfxPort_State_low);
               IfxPort_setPinState(LED2, IfxPort_State_low);
               IfxPort_setPinState(LED3, IfxPort_State_high);
           }
           else /* Turn off all LEDs */
           {
               IfxPort_setPinState(LED1, IfxPort_State_low);
               IfxPort_setPinState(LED2, IfxPort_State_low);
               IfxPort_setPinState(LED3, IfxPort_State_low);
           }
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME));    /* Wait 500 milliseconds            */
}


void initUltrasonic(void)
{
    /* Initialization of the Trigger & Echo Pins used in this example */
    IfxPort_setPinMode(TRIG_PIN, IfxPort_Mode_outputPushPullGeneral);
    IfxPort_setPinMode(ECHO_PIN, IfxPort_Mode_inputPullUp);

}

/* This function toggles the port pin and wait 500 milliseconds */
void GetDistance(void)
{
    IfxPort_setPinState(TRIG_PIN, IfxPort_State_low);
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME_5MICRO));    /* Wait 5 micro-seconds            */
    IfxPort_setPinState(TRIG_PIN, IfxPort_State_high);
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME_10MICRO));    /* Wait 10 micro-seconds */
    IfxPort_setPinState(TRIG_PIN, IfxPort_State_low);

    while(IfxPort_getPinState(ECHO_PIN) == 0) {}

    g_start_ticks = IfxStm_get(&MODULE_STM0);

    while(IfxPort_getPinState(ECHO_PIN) == 1) {}

    g_stop_ticks = IfxStm_get(&MODULE_STM0);

    g_total_ticks = (g_stop_ticks - g_start_ticks);
    g_freq = IfxStm_getFrequency(&MODULE_STM0);

    g_total_time = (float32)(g_total_ticks*1000*1000)/g_freq;

    g_distance = (uint16)((g_total_time/2) / 29.1);

    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME_50MICRO));    // Wait 50 micro-seconds

}
