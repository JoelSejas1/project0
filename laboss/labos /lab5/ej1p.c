#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

// Pines en Puerto Q
#define TRIG_PIN GPIO_PIN_2 // PQ2
#define ECHO_PIN GPIO_PIN_3 // PQ3

int main(void) {
    // Reloj a 120 MHz
    uint32_t ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | 
                                               SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    // Habilitar Puerto Q (Sensor) y Puertos N/F (LEDs)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOQ));

    // Configurar Sensor
    GPIOPinTypeGPIOOutput(GPIO_PORTQ_BASE, TRIG_PIN);
    GPIOPinTypeGPIOInput(GPIO_PORTQ_BASE, ECHO_PIN);

    // Configurar 4 LEDs de usuario como salida
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);

    while(1) {
        uint32_t ui32Count = 0;
        uint32_t timeout = 500000; // Timeout largo para no trabarse

        // 1. Generar Trigger (10us)
        GPIOPinWrite(GPIO_PORTQ_BASE, TRIG_PIN, 0);
        SysCtlDelay(ui32SysClock / 3000000); // 1us
        GPIOPinWrite(GPIO_PORTQ_BASE, TRIG_PIN, TRIG_PIN);
        SysCtlDelay((ui32SysClock / 3000000) * 10); // 10us
        GPIOPinWrite(GPIO_PORTQ_BASE, TRIG_PIN, 0);

        // 2. Esperar a que ECHO suba
        while(GPIOPinRead(GPIO_PORTQ_BASE, ECHO_PIN) == 0 && timeout > 0) {
            timeout--;
        }

        // 3. Si subió, medir cuánto tiempo dura arriba
        if(timeout > 0) {
            while(GPIOPinRead(GPIO_PORTQ_BASE, ECHO_PIN) != 0 && ui32Count < 100000) {
                ui32Count++;
            }
        }

        // 4. Lógica de LEDs: Si ui32Count es pequeño (objeto cerca), prender LEDs
        // El valor 5000 es un aproximado para 10-15cm a 120MHz
        if(ui32Count > 10 && ui32Count < 5000) {
            // PRENDER TODOS LOS LEDS
            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0xFF);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, 0xFF);
        } else {
            // APAGAR LEDS
            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, 0);
        }

        // Pausa de 100ms para estabilizar
        SysCtlDelay(ui32SysClock / 30); 
    }
}