import RPi.GPIO as GPIO
from pynput import keyboard
import time

PWM1, IN1, IN2 = 12, 23, 24  
PWM2, IN3, IN4 = 13, 27, 22  

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup([PWM1, IN1, IN2, PWM2, IN3, IN4], GPIO.OUT)

pwm_izq = GPIO.PWM(PWM1, 1000)
pwm_der = GPIO.PWM(PWM2, 1000)
pwm_izq.start(0)
pwm_der.start(0)

vel = 50.0
estado = 0 
presionado = set()

def mover_motores(izq, der):

    if izq == 1:       
        GPIO.output(IN1, GPIO.HIGH)
        GPIO.output(IN2, GPIO.LOW)
        pwm_izq.ChangeDutyCycle(vel)

    elif izq == -1:    
        GPIO.output(IN1, GPIO.LOW)
        GPIO.output(IN2, GPIO.HIGH)
        pwm_izq.ChangeDutyCycle(vel)

    else:              
        GPIO.output(IN1, GPIO.LOW)
        GPIO.output(IN2, GPIO.LOW)
        pwm_izq.ChangeDutyCycle(0)

    if der == 1:       
        GPIO.output(IN3, GPIO.HIGH)
        GPIO.output(IN4, GPIO.LOW)
        pwm_der.ChangeDutyCycle(vel)

    elif der == -1:    
        GPIO.output(IN3, GPIO.LOW)
        GPIO.output(IN4, GPIO.HIGH)
        pwm_der.ChangeDutyCycle(vel)

    else:              
        GPIO.output(IN3, GPIO.LOW)
        GPIO.output(IN4, GPIO.LOW)
        pwm_der.ChangeDutyCycle(0)

def movimiento():
    if 'w' in presionado:
        mover_motores(1, 1)
    elif 's' in presionado:
        mover_motores(-1, -1)
    elif 'a' in presionado:
        mover_motores(-1, 1) 
    elif 'd' in presionado:
        mover_motores(1, -1) 
    else:
        mover_motores(0, 0)

def al_presionar(key):
    global vel, estado
    try:
        k = key.char.lower()
        if k in ['w', 'a', 's', 'd']:
            presionado.add(k)
            movimiento()
        
        elif k == 'n':
            if estado == 0:
                vel = 100.0
                estado = 1
            elif estado == 1:
                vel = 70.0
                estado = 2
            else:
                vel = 50.0
                estado = 0
            print(f"\rVelocidad: {vel}%   ", end="")
            
    except AttributeError:
        if key == keyboard.Key.esc:
            return False 

def soltar(key):
    try:
        k = key.char.lower()
        if k in presionado:
            presionado.remove(k)
            movimiento()
    except:
        pass

print(" teclado ")
print("WASD para mover | N para ciclo velocidad | ESC para salir")

with keyboard.Listener(on_press=al_presionar, on_release=soltar) as listener:
    listener.join()

pwm_izq.stop()
pwm_der.stop()
GPIO.cleanup()
print("\ncerrar")
