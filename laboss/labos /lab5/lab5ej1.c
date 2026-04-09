#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/adc.h"

// Variable global para capturar el valor del potenciómetro (0-4095)
volatile uint32_t ui32ADCValue;
uint32_t ui32ClockFreq;

// Función de interrupción CORREGIDA para GCC
void Timer0A_Handler(void) {
    // Limpiar la interrupción del timer (Indispensable)
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // --- SECUENCIA PERSONALIZADA (Ejercicio 1) ---
    // Invertimos el estado del LED PN1 para visualizar la frecuencia de muestreo
    if(GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_1)) {
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0);
    } else {
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
    }
}

int main(void) {
    // 1. Configurar reloj a 120 MHz (Frecuencia máxima de la TM4C1294)
    ui32ClockFreq = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | 
                                        SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    // 2. Habilitar periféricos necesarios
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);   // ADC para potenciómetro
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);  // Puerto K (para pin PK3/AIN19)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);  // Puerto N (para LEDs de usuario)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); // Timer 0

    // Esperar a que los periféricos estén listos
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));

    // 3. Configurar Pin PK3 como entrada analógica (AIN19)
    GPIOPinTypeADC(GPIO_PORTK_BASE, GPIO_PIN_3);

    // 4. Configurar LED PN1 como salida
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

    // 5. Configurar ADC0: Secuenciador 3, disparo por procesador, 1 sola muestra
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH19 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);
    ADCIntClear(ADC0_BASE, 3);

    // 6. Configurar Timer 0 como periódico de 32 bits
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    
    // IMPORTANTE: Aunque en GCC registramos en el startup, 
    // esta función ayuda a habilitar el subtimer A.
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER0A);
    IntMasterEnable(); // Habilita interrupciones globales
    
    TimerEnable(TIMER0_BASE, TIMER_A);

    while(1) {
        // A. Disparar la conversión del ADC
        ADCProcessorTrigger(ADC0_BASE, 3);
        
        // B. Esperar a que termine la conversión
        while(!ADCIntStatus(ADC0_BASE, 3, false));
        ADCIntClear(ADC0_BASE, 3);
        
        // C. Leer el valor del potenciómetro (0 a 4095)
        ADCSequenceDataGet(ADC0_BASE, 3, (uint32_t *)&ui32ADCValue);

        // D. Calcular nuevo periodo para el Timer (Variar Fs)
        // Usamos una fórmula para que el parpadeo sea visible.
        // A menor valor de ADC, mayor periodo (parpadeo lento).
        // A mayor valor de ADC, menor periodo (parpadeo rápido).
        uint32_t ui32Period = (ui32ClockFreq / (ui32ADCValue + 10)) * 50; 
        
        // E. Actualizar el "techo" del Timer en tiempo real
        TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period);
        
        // Pequeño retardo para no saturar el ADC
        SysCtlDelay(ui32ClockFreq / 100); 
    }
}