import RPi.GPIO as GPIO
import time
import serial


# Motor 1
PWM1 = 12  
IN1  = 23  
IN2  = 24  

# Motor 2
PWM2 = 13  
IN3  = 27  
IN4  = 22  

#Boton
boton = 17


GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)


pines_motores = [PWM1, IN1, IN2, PWM2, IN3, IN4]
GPIO.setup(pines_motores, GPIO.OUT)
GPIO.setup(boton, GPIO.IN, pull_up_down=GPIO.PUD_UP)


pwm_m1 = GPIO.PWM(PWM1, 1000)
pwm_m2 = GPIO.PWM(PWM2, 1000)

pwm_m1.start(0)
pwm_m2.start(0)


#Abrir el archivo que tendra la velocidad
#Cambiar el valor
#echo "1" > velo.txt
def velocidad():
    try:
        
        with open("velo.txt", "r") as f:
            valor = f.read().strip()
            return float(valor)
    except:
        return 1.0
        

# --- Configuración UART ---
try:
    ser = serial.Serial('/dev/serial0', 9600, timeout=1)
    print("Comunicación UART lista. Esperando a la Tiva...")
except Exception as e:
    print(f"Error al abrir UART: {e}")


def activar_motor(motor_id):
    """Activa el motor indicado al 50% de duty cycle por 2 segundos"""
    
    ci_du = velocidad()
    if motor_id == 1:
        print(">>> Ejecutando: MOTOR 1 al 50%")
        GPIO.output(IN1, GPIO.HIGH)
        GPIO.output(IN2, GPIO.LOW)
        pwm_m1.ChangeDutyCycle(ci_du)
        time.sleep(5)
        pwm_m1.ChangeDutyCycle(0)
    
    elif motor_id == 2:
        print(">>> Ejecutando: MOTOR 2 al 50%")
        GPIO.output(IN3, GPIO.HIGH)
        GPIO.output(IN4, GPIO.LOW)
        pwm_m2.ChangeDutyCycle(ci_du)
        time.sleep(5)
        pwm_m2.ChangeDutyCycle(0)

# --- Bucle Principal ---
try:
    while True:
        if ser.in_waiting > 0:
            # Leer línea desde la Tiva y limpiar espacios/saltos
            dato_recibido = ser.readline().decode('utf-8').strip()
            
            if dato_recibido == "motor1":
                activar_motor(1)
            elif dato_recibido == "motor2":
                activar_motor(2)
            else:
                if dato_recibido: # Si llegó algo pero no es motor1/2
                    print(f"Comando desconocido: {dato_recibido}")
		
        if GPIO.input(boton) == GPIO.HIGH:
            print("Buzzer")
            ser.write(b'buzzer\n')
            time.sleep(0.3)
        time.sleep(0.01) 

except KeyboardInterrupt:
    print("\nDeteniendo programa...")
finally:
    pwm_m1.stop()
    pwm_m2.stop()
    GPIO.cleanup()
    ser.close()
    print("GPIO Limpio y puerto serial cerrado.")
