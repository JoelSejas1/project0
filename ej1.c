#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"
// Variable para guardar la frecuencia del reloj
int tiempo;

// --- MANEJADOR DE INTERRUPCIÓN ---
// Este nombre debe estar en el archivo startup_gcc.c
void Timer0A_Handler(void)
{
    // 1. Limpiar la interrupción (Obligatorio)
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // 2. Leer el estado actual del LED PN1 y conmutarlo (Toggle)
    if(GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_1))
    {
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0);
    }
    else
    {
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
    }
}

int main(void)
{
    // 1. Configurar reloj a 120 MHz
    tiempo = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_240), 120000000);

    // 2. Configurar el Puerto N (LED D1 es PN1)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

    // 3. Configurar Timer 0 
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));
    
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    // 4. Cargar el valor del tiempo 
    // Caso A: 1 segundo  -> tiempo
    // Caso B: 2 segundos -> tiempo * 2
    // Caso C: 5 segundos -> tiempo * 5
    TimerLoadSet(TIMER0_BASE, TIMER_A, tiempo - 1); 

    // 5. Configurar Interrupciones
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER0A);                          
    IntMasterEnable();                              

    // 6. Iniciar el Timer
    TimerEnable(TIMER0_BASE, TIMER_A);

    while(1)
    {
    }
}