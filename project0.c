#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"

#define BUZZER_PIN GPIO_PIN_1 // PF1
#define TRIG_PIN   GPIO_PIN_2 // PQ2
#define ECHO_PIN   GPIO_PIN_3 // PQ3

void delayUs(uint32_t ui32SysClock, uint32_t us) {
    SysCtlDelay((ui32SysClock / 3000000) * us);
}

void UART4_SendString(char *pcStr) {
    while(*pcStr) {
        UARTCharPut(UART4_BASE, *pcStr++);
    }
}

uint32_t medirDistancia(uint32_t ui32SysClock) {
    uint32_t ui32Count = 0;
    uint32_t timeout = 100000; 

    // 1. Disparo
    GPIOPinWrite(GPIO_PORTQ_BASE, TRIG_PIN, 0);
    delayUs(ui32SysClock, 2);
    GPIOPinWrite(GPIO_PORTQ_BASE, TRIG_PIN, TRIG_PIN);
    delayUs(ui32SysClock, 10);
    GPIOPinWrite(GPIO_PORTQ_BASE, TRIG_PIN, 0);

    // 2. Esperar que ECHO suba (con escape de seguridad)
    while(GPIOPinRead(GPIO_PORTQ_BASE, ECHO_PIN) == 0 && timeout > 0) timeout--;
    
    if(timeout == 0) return 999; // Si no hay eco, devuelve distancia lejana

    // 3. Medir tiempo que ECHO está en alto
    while(GPIOPinRead(GPIO_PORTQ_BASE, ECHO_PIN) != 0 && ui32Count < 40000) {
        ui32Count++;
        delayUs(ui32SysClock, 1);
    }

    return (ui32Count / 10); 
}

int main(void) {
    char buffer[15];
    int i = 0;
    uint32_t ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | 
                                               SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    // Habilitar Puertos: K (UART), J (Botones), F (Buzzer), Q (Sensor)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART4);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ); // PUERTO Q

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOQ));

    // UART4 Config
    GPIOPinConfigure(GPIO_PK0_U4RX);
    GPIOPinConfigure(GPIO_PK1_U4TX);
    GPIOPinTypeUART(GPIO_PORTK_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART4_BASE, ui32SysClock, 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // GPIO Config
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, BUZZER_PIN);
    GPIOPinTypeGPIOOutput(GPIO_PORTQ_BASE, TRIG_PIN);
    GPIOPinTypeGPIOInput(GPIO_PORTQ_BASE, ECHO_PIN);

    // Botones con Pull-up
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    while(1) {
        // --- MEDIR SENSOR ---
        uint32_t dist = medirDistancia(ui32SysClock);
        
        if(dist <= 30 && dist > 0) {
            UART4_SendString("OBJETO_DETECTADO\n");
            // Feedback: Prender LED o Buzzer brevemente para saber que detectó
            GPIOPinWrite(GPIO_PORTF_BASE, BUZZER_PIN, BUZZER_PIN);
            SysCtlDelay(ui32SysClock / 2); 
            GPIOPinWrite(GPIO_PORTF_BASE, BUZZER_PIN, 0);
        }

        // --- BOTONES ---
        if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == 0) {
            UART4_SendString("motor1\n");
            while(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == 0);
        }
        if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) == 0) {
            UART4_SendString("motor2\n");
            while(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) == 0);
        }

        // --- UART RECEIVE ---
        if (UARTCharsAvail(UART4_BASE)) {
            char c = UARTCharGet(UART4_BASE);
            if (c != '\n' && i < 14) { buffer[i++] = c; } 
            else {
                buffer[i] = '\0';
                if (strcmp(buffer, "buzzer") == 0) {
                    GPIOPinWrite(GPIO_PORTF_BASE, BUZZER_PIN, BUZZER_PIN);
                    SysCtlDelay(ui32SysClock / 2); 
                    GPIOPinWrite(GPIO_PORTF_BASE, BUZZER_PIN, 0);
                }
                i = 0;
            }
        }
        SysCtlDelay(ui32SysClock / 200); // Evitar saturación
    }
}