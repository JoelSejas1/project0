import RPi.GPIO as GPIO
import time
import serial

PWM1, IN1, IN2 = 12, 23, 24
PWM2, IN3, IN4 = 13, 27, 22
boton = 17

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

GPIO.setup([PWM1, IN1, IN2, PWM2, IN3, IN4], GPIO.OUT)
GPIO.setup(boton, GPIO.IN, pull_up_down=GPIO.PUD_UP)

pwm_m1 = GPIO.PWM(PWM1, 1000)
pwm_m2 = GPIO.PWM(PWM2, 1000)
pwm_m1.start(0)
pwm_m2.start(0)

m1_activo = 0
m2_activo = 0

def velocidad():
    try:
        with open("velo.txt", "r") as f:
            return float(f.read().strip())
    except:
        return 50.0

def revisar_giro():
    global m1_activo, m2_activo
    try:
        with open("grados.txt", "r") as f:
            contenido = f.read().strip().upper()
        if not contenido:
            return
            
        # Limpiamos el archivo para no repetir la accion
        with open("grados.txt", "w") as f:
            f.write("")
            
        direccion = contenido[0]  # I o D
        grados = float(contenido[1:])
        
        # Usamos tu valor de 1.4 segundos por cada 90 grados
        tiempo_giro = grados * (1.4 / 90.0)
        
        v = velocidad()
        if direccion == 'I':
            print(f"Giro archivo: {grados} izq")
            GPIO.output(IN1, GPIO.HIGH)
            GPIO.output(IN2, GPIO.LOW)
            pwm_m1.ChangeDutyCycle(v)
            
            GPIO.output(IN3, GPIO.LOW)
            GPIO.output(IN4, GPIO.LOW)
            pwm_m2.ChangeDutyCycle(0)
            time.sleep(tiempo_giro)
            
        elif direccion == 'D':
            print(f"Giro archivo: {grados} der")
            GPIO.output(IN1, GPIO.LOW)
            GPIO.output(IN2, GPIO.LOW)
            pwm_m1.ChangeDutyCycle(0)
            
            GPIO.output(IN3, GPIO.HIGH)
            GPIO.output(IN4, GPIO.LOW)
            pwm_m2.ChangeDutyCycle(v)
            time.sleep(tiempo_giro)
            
        # Seguir funcionando al frente
        m1_activo = 1
        m2_activo = 1
        
    except Exception:
        # Pasa en silencio si el archivo no existe o falla el parseo
        pass

print("Intentando abrir puerto serial...")
try:
    ser = serial.Serial('/dev/serial0', 9600, timeout=0.1)
    print("Puerto serial abierto correctamente.")
except Exception as e:
    print(f"Error critico abriendo serial: {e}")

print("Entrando en bucle principal. Presiona el boton o envia datos...")

try:
    while True:
        if ser.in_waiting > 0:
            dato = ser.readline().decode('utf-8', errors='ignore').strip()
            if dato:
                print(f"Recibido por UART: {dato}")
                if dato == "motor1":
                    print("M1")
                    m1_activo = 1
                elif dato == "motor2":
                    print("M2")
                    m2_activo = 1
                elif dato == "OBJETO_DETECTADO":
                    print("girando...")
                    v = velocidad()
                    GPIO.output(IN1, GPIO.HIGH)
                    GPIO.output(IN2, GPIO.LOW)
                    pwm_m1.ChangeDutyCycle(v)
                    
                    GPIO.output(IN3, GPIO.LOW)
                    GPIO.output(IN4, GPIO.LOW)
                    pwm_m2.ChangeDutyCycle(0)
                    
                    time.sleep(1.0) 
                    
                    m1_activo = 1
                    m2_activo = 1

        v = velocidad()
        revisar_giro()

        if m1_activo == 1:
            GPIO.output(IN1, GPIO.HIGH)
            GPIO.output(IN2, GPIO.LOW)
            pwm_m1.ChangeDutyCycle(v)
        else:
            pwm_m1.ChangeDutyCycle(0)

        if m2_activo == 1:
            GPIO.output(IN3, GPIO.HIGH)
            GPIO.output(IN4, GPIO.LOW)
            pwm_m2.ChangeDutyCycle(v)
        else:
            pwm_m2.ChangeDutyCycle(0)

        if GPIO.input(boton) == GPIO.LOW:
            print("Boton presionado: Enviando buzzer a Tiva")
            ser.write(b'buzzer\n')
            time.sleep(0.3)
            
        time.sleep(0.01)

except KeyboardInterrupt:
    print("\nPrograma interrumpido por el usuario.")
finally:
    pwm_m1.stop()
    pwm_m2.stop()
    GPIO.cleanup()
    if 'ser' in locals():
        ser.close()
    print("Limpieza completada.")