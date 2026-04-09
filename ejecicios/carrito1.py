import RPi.GPIO as GPIO
import time
import serial

BUZZER_PIN = 17   
TRIG_PIN   = 23   
ECHO_PIN   = 24   
BOTON1     = 27   
BOTON2     = 22   

ser = serial.Serial(
    port='/dev/serial0',
    baudrate=9600,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=0.1
)

def setup():
    GPIO.setmode(GPIO.BCM)
    GPIO.setwarnings(False)
    
    GPIO.setup(BUZZER_PIN, GPIO.OUT)
    GPIO.setup(TRIG_PIN, GPIO.OUT)
    
    GPIO.setup(ECHO_PIN, GPIO.IN)
    GPIO.setup(BOTON1, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    GPIO.setup(BOTON2, GPIO.IN, pull_up_down=GPIO.PUD_UP)

def medir_distancia():
    GPIO.output(TRIG_PIN, False)
    time.sleep(0.000002) 
    GPIO.output(TRIG_PIN, True)
    time.sleep(0.000010) 
    GPIO.output(TRIG_PIN, False)

    inicio = time.time()
    fin = time.time()

    while GPIO.input(ECHO_PIN) == 0:
        inicio = time.time()
    
    while GPIO.input(ECHO_PIN) == 1:
        fin = time.time()

    duracion = fin - inicio
    distancia = (duracion * 34300) / 2
    return distancia

def activar_buzzer():
    GPIO.output(BUZZER_PIN, True)
    time.sleep(0.5) 
    GPIO.output(BUZZER_PIN, False)

def main():
    setup()
    print("Raspberry Pi lista. Esperando eventos...")
    
    try:
        while True:
            dist = medir_distancia()
            
            if 0 < dist <= 30:
                ser.write(b"OBJETO_DETECTADO\n")
                activar_buzzer()

            if GPIO.input(BOTON1) == GPIO.LOW:
                ser.write(b"motor1\n")
                while GPIO.input(BOTON1) == GPIO.LOW: 
                    time.sleep(0.01)

            if GPIO.input(BOTON2) == GPIO.LOW:
                ser.write(b"motor2\n")
                while GPIO.input(BOTON2) == GPIO.LOW: 
                    time.sleep(0.01)

            if ser.in_waiting > 0:
                linea = ser.readline().decode('utf-8').strip()
                if linea == "buzzer":
                    activar_buzzer()

            time.sleep(0.01) 

    except KeyboardInterrupt:
        GPIO.cleanup()
        ser.close()

if __name__ == "__main__":
    main()