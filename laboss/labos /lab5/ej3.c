#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

// Variables globales
int tiempo;
volatile int s = 0; 
int v = 0;
bool botonPresionado = false;

void Timer0A_Handler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    
    s++;
    if(s > 400) s = 1;

    GPIOPinWrite(GPIO_PORTN_BASE, (GPIO_PIN_0 | GPIO_PIN_1), 0);
    GPIOPinWrite(GPIO_PORTF_BASE, (GPIO_PIN_0 | GPIO_PIN_4), 0);
    
    if(s > 0 && s <= 100)       GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1); 
    else if(s > 100 && s <= 200) GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0); 
    else if(s > 200 && s <= 300) GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_PIN_4); 
    else if(s > 300 && s <= 400) GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_0); 
}

int main(void)
{
    tiempo = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | 
                                 SYSCTL_USE_PLL | SYSCTL_CFG_VCO_240), 120000000);

    // LEDs
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);

    // boton
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ));
    
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_1);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Timer
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, (tiempo / 50) - 1); 

    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER0A); 
    IntMasterEnable();      
    TimerEnable(TIMER0_BASE, TIMER_A);

    while(1)
    {
        if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) == 0) 
        {
            if(!botonPresionado) 
            {
                v++;
                if(v > 4) v = 1;

                // Actualizar velocidad
                if (v == 1)      TimerLoadSet(TIMER0_BASE, TIMER_A, (tiempo / 100) - 1);
                else if (v == 2) TimerLoadSet(TIMER0_BASE, TIMER_A, (tiempo / 200) - 1);
                else if (v == 3) TimerLoadSet(TIMER0_BASE, TIMER_A, (tiempo / 400) - 1);
                else if (v == 4) TimerLoadSet(TIMER0_BASE, TIMER_A, (tiempo / 50) - 1);
                
                botonPresionado = true; 
            }
        }
        else
        {
            botonPresionado = false; 
        }
    }
}