import RPi.GPIO as GPIO
import time
import random
# Importación para el sensor real (descomentar cuando se use hardware físico)
# import adafruit_dht
# import board

HEATER_LED_PIN = 23 # LED rojo
FAN_PIN = 24

def setup():
    GPIO.setwarnings(False)
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(HEATER_LED_PIN, GPIO.OUT)
    GPIO.setup(FAN_PIN, GPIO.OUT)

def obtener_temperatura_aleatoria():
    # Simula la temperatura inicial
    return round(random.uniform(5.0, 30.0), 1)

"""
# Lógica para sensor DHT22 (Reemplazo para el paso final)
dht_device = adafruit_dht.DHT22(board.D4)
def obtener_temperatura_dht():
    try:
        return dht_device.temperature
    except RuntimeError as error:
        return None
"""

def controlar_invernadero(temp):
    if temp is None:
        return

    print(f"Temperatura actual: {temp} °C")
    
    if temp < 12.0:
        print("-> Calentador ENCENDIDO, Ventilador APAGADO")
        GPIO.output(HEATER_LED_PIN, GPIO.HIGH)
        GPIO.output(FAN_PIN, GPIO.LOW)
    elif temp > 20.0:
        print("-> Ventilador ENCENDIDO, Calentador APAGADO")
        GPIO.output(HEATER_LED_PIN, GPIO.LOW)
        GPIO.output(FAN_PIN, GPIO.HIGH)
    else:
        print("-> Temperatura Óptima. Ambos APAGADOS")
        GPIO.output(HEATER_LED_PIN, GPIO.LOW)
        GPIO.output(FAN_PIN, GPIO.LOW)

def main():
    setup()
    try:
        while True:
            temp = obtener_temperatura_aleatoria()
            # temp = obtener_temperatura_dht() # Usar esta línea con hardware real
            controlar_invernadero(temp)
            time.sleep(2)
    except KeyboardInterrupt:
        GPIO.cleanup()

if __name__ == '__main__':
    main()
