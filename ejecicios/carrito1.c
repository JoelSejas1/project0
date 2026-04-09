#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"

#define MOTOR_PORT      GPIO_PORTK_BASE
#define IN1 GPIO_PIN_4
#define IN2 GPIO_PIN_5
#define IN3 GPIO_PIN_6
#define IN4 GPIO_PIN_7
#define BOTON GPIO_PIN_0

volatile uint32_t velocidad = 50; 
int m1_activo = 0;
int m2_activo = 0;

void UART4_Init(uint32_t ui32SysClock) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART4);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    GPIOPinConfigure(GPIO_PK0_U4RX);
    GPIOPinConfigure(GPIO_PK1_U4TX);
    GPIOPinTypeUART(GPIO_PORTK_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART4_BASE, ui32SysClock, 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
}

void PWM_Init(uint32_t ui32SysClock) {
    // El PWM0 usa el puerto F (PF0 y PF1)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    
    GPIOPinConfigure(GPIO_PF0_M0PWM0);
    GPIOPinConfigure(GPIO_PF1_M0PWM1);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    uint32_t ui32PWMClock = ui32SysClock / 64; 
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ui32PWMClock / 1000); 

    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT | PWM_OUT_1_BIT, true);
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
}

void set_pwm_duty(uint32_t duty, uint32_t out_bit) {
    if (duty > 99) duty = 99;
    uint32_t period = PWMGenPeriodGet(PWM0_BASE, PWM_GEN_0);
    PWMPulseWidthSet(PWM0_BASE, out_bit, (duty * period) / 100);
}

int main(void) {
    uint32_t ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    UART4_Init(ui32SysClock);
    PWM_Init(ui32SysClock);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    GPIOPinTypeGPIOOutput(MOTOR_PORT, IN1 | IN2 | IN3 | IN4);
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, BOTON);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, BOTON, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    char buffer[20];
    int idx = 0;

    while(1) {
        if (UARTCharsAvail(UART4_BASE)) {
            char c = UARTCharGet(UART4_BASE);
            if (c != '\n' && idx < 19) {
                buffer[idx++] = c;
            } else {
                buffer[idx] = '\0';
                if (strcmp(buffer, "motor1") == 0) m1_activo = 1;
                else if (strcmp(buffer, "motor2") == 0) m2_activo = 1;
                else if (strcmp(buffer, "OBJETO_DETECTADO") == 0) { m1_activo = 0; m2_activo = 0; }
                else if (isdigit(buffer[0])) { velocidad = atoi(buffer); }
                idx = 0;
            }
        }
        if (m1_activo) {
            GPIOPinWrite(MOTOR_PORT, IN1, IN1);
            GPIOPinWrite(MOTOR_PORT, IN2, 0);
            set_pwm_duty(velocidad, PWM_OUT_0);
        } else {
            set_pwm_duty(0, PWM_OUT_0);
        }

        if (m2_activo) {
            GPIOPinWrite(MOTOR_PORT, IN3, IN3);
            GPIOPinWrite(MOTOR_PORT, IN4, 0);
            set_pwm_duty(velocidad, PWM_OUT_1);
        } else {
            set_pwm_duty(0, PWM_OUT_1);
        }

        if (GPIOPinRead(GPIO_PORTJ_BASE, BOTON) == 0) {
            const char *msg = "buzzer\n";
            while(*msg) { UARTCharPut(UART4_BASE, *msg++); }
            SysCtlDelay(ui32SysClock / 10); 
        }
    }
}