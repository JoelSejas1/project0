#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

int main(void)
{
    // Variable solicitada por el PDF para almacenar el valor decimal
    int counter = 0;
    
    // Variable para crear un pequeño retardo antirrebote (debouncing)
    uint32_t retardo_rebote = 1600000;

    // 1. Habilitar la energia de los puertos a usar (J para botones, N y F para LEDs)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ); 
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION); 
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); 

    // Esperar a que los puertos confirmen que estan listos
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ) || 
          !SysCtlPeripheralReady(SYSCTL_PERIPH_GPION) || 
          !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {                                                    
    }                                                    

    // 2. Configurar los LEDs como SALIDAS
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1); 
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4); 

    // 3. Configurar los Botones como ENTRADAS con Pull-Up
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);  
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); 

    // Bucle infinito del contador
    while(1)                                             
    {                                                    
        // Leer el estado de los botones (0 es PRESIONADO)
        int sw1_presionado = (GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == 0); 
        int sw2_presionado = (GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) == 0); 

        // LOGICA DE INCREMENTO (SW1)
        if(sw1_presionado)
        {
            if(counter < 15)
            {
                counter++;
            }
            SysCtlDelay(retardo_rebote);
        }

        // LOGICA DE DECREMENTO (SW2)
        else if(sw2_presionado)
        {
            if(counter > 0)
            {
                counter--;
            }
            SysCtlDelay(retardo_rebote);
        }

        // =====================================================================
        // LOGICA DE TRADUCCION A BINARIO EN LOS LEDS 
        // =====================================================================

        // LED D1 (Representa el bit 0, que vale 1)
        if((counter & 1) != 0)                                   
        {
            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
        }
        else
        {
            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0x0);
        }

        // LED D2 (Representa el bit 1, que vale 2)
        if((counter & 2) != 0)                                   
        {
            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
        }
        else
        {
            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x0);
        }

        // LED D3 (Representa el bit 2, que vale 4)
        if((counter & 4) != 0)                                   
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_PIN_4);
        }
        else
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0x0);
        }

        // LED D4 (Representa el bit 3, que vale 8)
        if((counter & 8) != 0)                                   
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_0);
        }
        else
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x0);
        }
    }                                                    
}