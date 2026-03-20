#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "config.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#ifndef VELOCIDAD
#define VELOCIDAD 1.0
#endif
uint32_t tiempo;
volatile uint8_t s = 0;

// --- HANDLER: SECUENCIA DE 3 ESTADOS ---
void Timer0A_Handler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    s++;
    if(s > 3) s = 1;

    // Apagar todos los LEDs involucrados
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);

    // Secuencia de 3 estados
    switch(s)
    {
        case 1: GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1); break; // LED D1
        case 2: GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0); break; // LED D2
        case 3: GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_PIN_4); break; // LED D3
    }
}

int main(void)
{
    tiempo = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_240), 120000000);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    // --- ESTA LÍNEA SERÁ MODIFICADA POR EL SCRIPT SSH ---
    // El valor 1.0 es el que cambiaremos dinámicamente
    TimerLoadSet(TIMER0_BASE, TIMER_A,tiempo * VELOCIDAD - 1); 

    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER0A); 
    IntMasterEnable();      
    TimerEnable(TIMER0_BASE, TIMER_A);

    while(1) {}
}