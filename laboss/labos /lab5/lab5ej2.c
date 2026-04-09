#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"

void UART4_Init(void) {
    // 1. Habilitar UART4 y Puerto K
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART4);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);

    // Esperar a que estén listos
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART4));

    // 2. Configurar el multiplexado de pines (Pin Muxing)
    // PK0 es U4RX y PK1 es U4TX
    GPIOPinConfigure(GPIO_PK0_U4RX);
    GPIOPinConfigure(GPIO_PK1_U4TX);
    
    // 3. Configurar los pines para funciones de UART
    GPIOPinTypeUART(GPIO_PORTK_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // 4. Configurar UART: 9600 baudios, 8-N-1
    // Usamos 120MHz que es la frecuencia que seteamos en el main
    UARTConfigSetExpClk(UART4_BASE, 120000000, 9600,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));
}

void Control_LEDs(int distancia) {
    // Apagamos los LEDs de los puertos N y F antes de evaluar
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1 | GPIO_PIN_0, 0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, 0);

    if (distancia > 10) {
        // Todo apagado
    } else if (distancia <= 10 && distancia > 8) {
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
    } else if (distancia <= 8 && distancia > 6) {
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1 | GPIO_PIN_0, 0xFF);
    } else if (distancia <= 6 && distancia > 4) {
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1 | GPIO_PIN_0, 0xFF);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_PIN_4);
    } else if (distancia <= 4) {
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1 | GPIO_PIN_0, 0xFF);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, 0xFF);
    }
}

int main(void) {
    uint32_t ui32ClockFreq = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | 
                                                SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);
    
    UART4_Init();
    
    // Habilitar Puertos de LEDs (N y F)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1 | GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);

    char buffer[10];
    int i = 0;

    while(1) {
        // Cambiamos UART0_BASE por UART4_BASE
        if(UARTCharsAvail(UART4_BASE)) {
            char c = UARTCharGet(UART4_BASE);
            if(c == '\n' || i >= 9) { 
                buffer[i] = '\0';
                int distRecibida = atoi(buffer);
                Control_LEDs(distRecibida);
                i = 0;
            } else {
                buffer[i++] = c;
            }
        }
    }
}