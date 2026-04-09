import RPi.GPIO as GPIO
import time

# Definición de pines (BCM)
LED1_PIN = 17
LED2_PIN = 27
BTN_PIN = 22

# Variables globales
estado_actual = 1
ultimo_cambio = 0
estado_led = False

def setup():
    GPIO.setwarnings(False)
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(LED1_PIN, GPIO.OUT)
    GPIO.setup(LED2_PIN, GPIO.OUT)
    # Configuramos el botón con resistencia Pull-Up
    GPIO.setup(BTN_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    # Interrupción para detectar el botón
    GPIO.add_event_detect(BTN_PIN, GPIO.FALLING, callback=cambiar_estado_btn, bouncetime=300)

def cambiar_estado_btn(channel):
    global estado_actual
    estado_actual += 1
    if estado_actual > 4:
        estado_actual = 1
    print(f"Cambiando al Estado {estado_actual}")
    # Limpiamos los LEDs al cambiar de estado
    GPIO.output(LED1_PIN, GPIO.LOW)
    GPIO.output(LED2_PIN, GPIO.LOW)

def ejecutar_estado():
    global ultimo_cambio, estado_led
    tiempo_actual = time.time()

    if estado_actual == 1:
        if tiempo_actual - ultimo_cambio >= 1.0:
            estado_led = not estado_led
            GPIO.output(LED1_PIN, estado_led)
            GPIO.output(LED2_PIN, not estado_led)
            ultimo_cambio = tiempo_actual

    elif estado_actual == 2:
        if tiempo_actual - ultimo_cambio >= 2.0:
            estado_led = not estado_led
            GPIO.output(LED1_PIN, estado_led)
            GPIO.output(LED2_PIN, estado_led)
            ultimo_cambio = tiempo_actual

    elif estado_actual == 3:
        GPIO.output(LED1_PIN, GPIO.HIGH)
        GPIO.output(LED2_PIN, GPIO.HIGH)

    elif estado_actual == 4:
        GPIO.output(LED1_PIN, GPIO.LOW)
        GPIO.output(LED2_PIN, GPIO.LOW)

def main():
    setup()
    try:
        while True:
            ejecutar_estado()
            time.sleep(0.01) # Pequeño delay para no saturar el CPU
    except KeyboardInterrupt:
        GPIO.cleanup()

if __name__ == '__main__':
    main()
