#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

int tiempo;
volatile int s = 0; // Variable para la secuencia (1 a 4)

// --- MANEJADOR DE INTERRUPCIÓN ---
void Timer0A_Handler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    s++;
    if(s > 4) s = 1;

    // Apagar todos los LEDs antes de mostrar el siguiente estado
    GPIOPinWrite(GPIO_PORTN_BASE, (GPIO_PIN_0 | GPIO_PIN_1), 0);
    GPIOPinWrite(GPIO_PORTF_BASE, (GPIO_PIN_0 | GPIO_PIN_4), 0);


    switch(s)
    {
        case 1: GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1); break; // D1
        case 2: GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0); break; // D2
        case 3: GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_PIN_4); break; // D3
        case 4: GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_0); break; // D4
    }
}

int main(void)
{
    // 1. Reloj a 120 MHz
    tiempo = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_240), 120000000);

    // 2. Configurar Puertos N y F para los 4 LEDs
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
    
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1); // D1, D2
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4); // D3, D4

    // 3. Configurar Timer 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));
    
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    // 4. Carga de Tiempo: 2 SEGUNDOS
    TimerLoadSet(TIMER0_BASE, TIMER_A, (tiempo * 2) - 1); 

    // 5. Configurar Interrupciones
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER0A); 
    IntMasterEnable();      

    TimerEnable(TIMER0_BASE, TIMER_A);

    while(1)
    {

    }
}