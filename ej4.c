#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

// --- VARIABLES GLOBALES ---
uint32_t tiempo;
volatile uint8_t c = 0; // Contador binario 0-15
bool boton = false;

// --- HANDLER: Lógica del Contador Binario ---
void Timer0A_Handler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Incrementar contador y resetear al llegar a 16 (0 a 15)
    c++;
    if(c > 15) c = 0;

    // Escribir en los LEDs representando bits:
    // Bit 0 (LSB): PN1 (D1)
    // Bit 1:       PN0 (D2)
    // Bit 2:       PF4 (D3)
    // Bit 3 (MSB): PF0 (D4)
    
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, (c & 0x01) ? GPIO_PIN_1 : 0);
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, (c & 0x02) ? GPIO_PIN_0 : 0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, (c & 0x04) ? GPIO_PIN_4 : 0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, (c & 0x08) ? GPIO_PIN_0 : 0);
}

int main(void)
{
    // 1. Reloj a 120 MHz
    tiempo = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | 
                                       SYSCTL_USE_PLL | SYSCTL_CFG_VCO_240), 120000000);

    // 2. Configurar LEDs (Puertos N y F)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
    
    // Desbloqueo de PF0 (Necesario para D4)
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);

    // 3. Configurar Botón USR_SW2 (PJ1)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ));
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_1);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // 4. Configurar Timer 0 a 1.5 Segundos inicialmente
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    // 1.5s = tiempo * 1.5 -> (tiempo * 3) / 2
    TimerLoadSet(TIMER0_BASE, TIMER_A, ((tiempo * 3) / 2) - 1);

    // 5. Interrupciones
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER0A); 
    IntMasterEnable();      
    TimerEnable(TIMER0_BASE, TIMER_A);

    while(1)
    {
        // Lógica del Botón para cambiar intervalo (Ejercicio 4)
        if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) == 0) // Presionado
        {
            if(!boton)
            {
                // Cambiar a 3 segundos
                TimerLoadSet(TIMER0_BASE, TIMER_A, (tiempo * 3) - 1);
                boton = true;
            }
        }
        else // Suelto
        {
            if(boton)
            {
                // Volver a 1.5 segundos
                TimerLoadSet(TIMER0_BASE, TIMER_A, ((tiempo * 3) / 2) - 1);
                boton = false;
            }
        }
    }
}