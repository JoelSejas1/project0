import RPi.GPIO as GPIO
import time

class ContadorBinario:
    def __init__(self, pines_led, pin_btn_inc, pin_btn_dec):
        self.pines_led = pines_led
        self.pin_btn_inc = pin_btn_inc
        self.pin_btn_dec = pin_btn_dec
        self.valor = 0
        
        GPIO.setwarnings(False)
        GPIO.setmode(GPIO.BCM)
        
        # Setup LEDs
        for pin in self.pines_led:
            GPIO.setup(pin, GPIO.OUT)
            GPIO.output(pin, GPIO.LOW)
            
        # Setup Botones
        GPIO.setup(self.pin_btn_inc, GPIO.IN, pull_up_down=GPIO.PUD_UP)
        GPIO.setup(self.pin_btn_dec, GPIO.IN, pull_up_down=GPIO.PUD_UP)
        
        GPIO.add_event_detect(self.pin_btn_inc, GPIO.FALLING, callback=self.incrementar, bouncetime=300)
        GPIO.add_event_detect(self.pin_btn_dec, GPIO.FALLING, callback=self.decrementar, bouncetime=300)
        self.actualizar_leds()

    def incrementar(self, channel):
        if self.valor == 15:
            self.valor = 0
        else:
            self.valor += 1
        self.actualizar_leds()

    def decrementar(self, channel):
        if self.valor > 0:
            self.valor -= 1
        self.actualizar_leds()

    def actualizar_leds(self):
        # Convertir a formato binario de 4 bits (ej: '0101')
        bin_str = format(self.valor, '04b')
        
        for i, bit in enumerate(bin_str):
            estado = GPIO.HIGH if bit == '1' else GPIO.LOW
            GPIO.output(self.pines_led[i], estado)
            
        print(f"Decimal: {self.valor} | Hex: {hex(self.valor).upper()} | Binario: {bin_str}")

    def run(self):
        try:
            while True:
                time.sleep(0.1)
        except KeyboardInterrupt:
            GPIO.cleanup()

if __name__ == '__main__':
    # Pines de ejemplo: MSB a LSB (bit más significativo a menos significativo)
    LEDS = [5, 6, 13, 19] 
    BTN_INC = 26
    BTN_DEC = 16
    
    contador = ContadorBinario(LEDS, BTN_INC, BTN_DEC)
    contador.run()
