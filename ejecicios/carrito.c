#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"

#define BUZZER_PIN GPIO_PIN_1 // PF1-BUZZER
#define TRIG_PIN   GPIO_PIN_2 // PQ2-SENSOR
#define ECHO_PIN   GPIO_PIN_3 // PQ3-SENSOR

void delayUs(uint32_t ui32SysClock, uint32_t us) { // RELOJ-DELAY
    SysCtlDelay((ui32SysClock / 3000000) * us); // RELOJ-DELAY
}

void UART4_SendString(char *pcStr) { // UART-TX 
    while(*pcStr) { // UART-TX
        UARTCharPut(UART4_BASE, *pcStr++); // UART-TX
    }
}

uint32_t medirDistancia(uint32_t ui32SysClock) { // SENSOR-FUNC
    uint32_t ui32Count = 0; // SENSOR-VAR
    uint32_t timeout = 100000; // SENSOR-VAR

    GPIOPinWrite(GPIO_PORTQ_BASE, TRIG_PIN, 0); // SENSOR-TRIG
    delayUs(ui32SysClock, 2); // SENSOR-TIME
    GPIOPinWrite(GPIO_PORTQ_BASE, TRIG_PIN, TRIG_PIN); // SENSOR-TRIG
    delayUs(ui32SysClock, 10); // SENSOR-TIME
    GPIOPinWrite(GPIO_PORTQ_BASE, TRIG_PIN, 0); // SENSOR-TRIG

    while(GPIOPinRead(GPIO_PORTQ_BASE, ECHO_PIN) == 0 && timeout > 0) timeout--; // SENSOR-ECHO
    
    if(timeout == 0) return 999; // SENSOR-TIMEOUT

    while(GPIOPinRead(GPIO_PORTQ_BASE, ECHO_PIN) != 0 && ui32Count < 40000) { // SENSOR-ECHO
        ui32Count++; // SENSOR-MEDIR
        delayUs(ui32SysClock, 1); // SENSOR-MEDIR
    }

    return (ui32Count / 10); // SENSOR-RESULTADO
}

int main(void) { // MAIN
    char buffer[15]; // UART-RX-VAR
    int i = 0; // UART-RX-VAR
    int ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | 
                                               SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000); // RELOJ-INIT

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART4); // UART-POWER
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK); // PORTK-POWER
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ); // PORTJ-POWER
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // PORTF-POWER
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ); // PORTQ-POWER

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOQ)); // PORTQ-READY

    GPIOPinConfigure(GPIO_PK0_U4RX); // UART-PINS
    GPIOPinConfigure(GPIO_PK1_U4TX); // UART-PINS
    GPIOPinTypeUART(GPIO_PORTK_BASE, GPIO_PIN_0 | GPIO_PIN_1); // UART-PINS
    UARTConfigSetExpClk(UART4_BASE, ui32SysClock, 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE)); // UART-INIT

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, BUZZER_PIN); // BUZZER-INIT
    GPIOPinTypeGPIOOutput(GPIO_PORTQ_BASE, TRIG_PIN); // SENSOR-TRIG-INIT
    GPIOPinTypeGPIOInput(GPIO_PORTQ_BASE, ECHO_PIN); // SENSOR-ECHO-INIT

    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1); // BOTONES-INIT
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); // BOTONES-PULLUP

    while(1) { // BUCLE
        uint32_t dist = medirDistancia(ui32SysClock); // SENSOR-MEDIR
        
        if(dist <= 30 && dist > 0) { // SENSOR-CHECK
            UART4_SendString("OBJETO_DETECTADO\n"); // UART-TX-OBJETO
            GPIOPinWrite(GPIO_PORTF_BASE, BUZZER_PIN, BUZZER_PIN); // BUZZER-ON
            SysCtlDelay(ui32SysClock / 2); // BUZZER-TIME
            GPIOPinWrite(GPIO_PORTF_BASE, BUZZER_PIN, 0); // BUZZER-OFF
        }

        if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == 0) { // BOTON1-CHECK
            UART4_SendString("motor1\n"); // UART-TX-M1
            while(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == 0); // BOTON1-DEBOUNCE
        }
        if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) == 0) { // BOTON2-CHECK
            UART4_SendString("motor2\n"); // UART-TX-M2
            while(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) == 0); // BOTON2-DEBOUNCE
        }

        if (UARTCharsAvail(UART4_BASE)) { // UART-RX-CHECK
            char c = UARTCharGet(UART4_BASE); // UART-RX-GET
            if (c != '\n' && i < 14) { buffer[i++] = c; } // UART-RX-BUF
            else { // UART-RX-CMD
                buffer[i] = '\0'; // UART-RX-BUF
                if (strcmp(buffer, "buzzer") == 0) { // BUZZER-CMD-CHECK
                    GPIOPinWrite(GPIO_PORTF_BASE, BUZZER_PIN, BUZZER_PIN); // BUZZER-ON
                    SysCtlDelay(ui32SysClock / 2); // BUZZER-TIME
                    GPIOPinWrite(GPIO_PORTF_BASE, BUZZER_PIN, 0); // BUZZER-OFF
                }
                i = 0; // UART-RX-RESET
            }
        }
        SysCtlDelay(ui32SysClock / 200); // BUCLE-PAUSA
    }
}