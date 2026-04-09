import RPi.GPIO as GPIO
import time

LEDS = [17, 27, 22, 10]
BTN_SEL = 9
BTN_TIME = 11

led_seleccionado = 0
tiempo_encendido = 1
tiempo_inicio_led = 0
led_activo = False

def setup():
    GPIO.setwarnings(False)
    GPIO.setmode(GPIO.BCM)
    for pin in LEDS:
        GPIO.setup(pin, GPIO.OUT)
        GPIO.output(pin, GPIO.LOW)
        
    GPIO.setup(BTN_SEL, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    GPIO.setup(BTN_TIME, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    
    GPIO.add_event_detect(BTN_SEL, GPIO.FALLING, callback=seleccionar_led, bouncetime=300)
    GPIO.add_event_detect(BTN_TIME, GPIO.FALLING, callback=aumentar_tiempo, bouncetime=300)

def seleccionar_led(channel):
    global led_seleccionado, tiempo_encendido, tiempo_inicio_led, led_activo
    
    # Apagamos el LED anterior
    GPIO.output(LEDS[led_seleccionado], GPIO.LOW)
    
    # Pasamos al siguiente LED
    led_seleccionado = (led_seleccionado + 1) % 4
    tiempo_encendido = 1
    tiempo_inicio_led = time.time()
    led_activo = True
    print(f"LED {led_seleccionado + 1} seleccionado. Tiempo: {tiempo_encendido}s")

def aumentar_tiempo(channel):
    global tiempo_encendido
    tiempo_encendido += 1
    print(f"Tiempo aumentado a: {tiempo_encendido}s para LED {led_seleccionado + 1}")

def main():
    global led_activo
    setup()
    try:
        while True:
            if led_activo:
                GPIO.output(LEDS[led_seleccionado], GPIO.HIGH)
                if (time.time() - tiempo_inicio_led) >= tiempo_encendido:
                    GPIO.output(LEDS[led_seleccionado], GPIO.LOW)
                    led_activo = False
            time.sleep(0.01)
    except KeyboardInterrupt:
        GPIO.cleanup()

if __name__ == '__main__':
    main()
