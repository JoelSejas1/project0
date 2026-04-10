import RPi.GPIO as GPIO
import time
import sys
import select
import tty
import termios
import serial

# Configuración de pines
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

print("Intentando abrir puerto serial para Tiva...")
try:
    ser = serial.Serial('/dev/serial0', 9600, timeout=0.1)
    print("Puerto serial abierto correctamente.")
except Exception as e:
    ser = None
    print(f"Aviso - Sin serial (el botón no enviará datos): {e}")

# Estado inicial
velocidad = 50.0
nitro_estado = 0  # 0: 50%, 1: 100%, 2: 70%

def configurar_motores(m1_in1, m1_in2, m2_in3, m2_in4):
    GPIO.output(IN1, m1_in1)
    GPIO.output(IN2, m1_in2)
    GPIO.output(IN3, m2_in3)
    GPIO.output(IN4, m2_in4)

def actualizar_pwm():
    pwm_m1.ChangeDutyCycle(velocidad)
    pwm_m2.ChangeDutyCycle(velocidad)

def detener():
    configurar_motores(GPIO.LOW, GPIO.LOW, GPIO.LOW, GPIO.LOW)
    pwm_m1.ChangeDutyCycle(0)
    pwm_m2.ChangeDutyCycle(0)

# Funcionalidad para leer el teclado en Linux sin detener el bucle
def es_tecla_presionada():
    return select.select([sys.stdin], [], [], 0) == ([sys.stdin], [], [])

print("\r\n--- CONTROL DEL CARRITO WASD ---")
print("Controles:")
print(" W : Adelante")
print(" S : Atrás")
print(" A : Izquierda (giro en el lugar)")
print(" D : Derecha (giro en el lugar)")
print(" X o Espacio : Detenerse")
print(" N : Nitro (Ciclo: 50% -> 100% -> 70%)")
print(" Q : Salir del programa")
print(f"Velocidad actual: {velocidad}%\r\n")

# Guardar la configuración original de la terminal
old_settings = termios.tcgetattr(sys.stdin)

try:
    # Poner la terminal en modo cbreak para lectura de teclas en tiempo real
    tty.setcbreak(sys.stdin.fileno())
    
    detener() # Iniciar completamente detenido
    
    while True:
        # 1. Lógica de control manual por teclado
        if es_tecla_presionada():
            tecla = sys.stdin.read(1).lower()
            
            if tecla == 'q':
                break
            elif tecla == 'w':
                configurar_motores(GPIO.HIGH, GPIO.LOW, GPIO.HIGH, GPIO.LOW)
                actualizar_pwm()
                sys.stdout.write("\rMovimiento: Adelante          ")
                sys.stdout.flush()
            elif tecla == 's':
                configurar_motores(GPIO.LOW, GPIO.HIGH, GPIO.LOW, GPIO.HIGH)
                actualizar_pwm()
                sys.stdout.write("\rMovimiento: Atrás             ")
                sys.stdout.flush()
            elif tecla == 'a':
                configurar_motores(GPIO.LOW, GPIO.HIGH, GPIO.HIGH, GPIO.LOW)
                actualizar_pwm()
                sys.stdout.write("\rMovimiento: Izquierda         ")
                sys.stdout.flush()
            elif tecla == 'd':
                configurar_motores(GPIO.HIGH, GPIO.LOW, GPIO.LOW, GPIO.HIGH)
                actualizar_pwm()
                sys.stdout.write("\rMovimiento: Derecha           ")
                sys.stdout.flush()
            elif tecla == ' ' or tecla == 'x':
                detener()
                sys.stdout.write("\rMovimiento: DETENIDO          ")
                sys.stdout.flush()
            elif tecla == 'n':
                if nitro_estado == 0:
                    velocidad = 100.0
                    nitro_estado = 1
                    sys.stdout.write(f"\rNITRO ACTIVADO: {velocidad}%          ")
                elif nitro_estado == 1:
                    velocidad = 70.0
                    nitro_estado = 2
                    sys.stdout.write(f"\rNITRO MEDIO: {velocidad}%             ")
                else:
                    velocidad = 50.0
                    nitro_estado = 0
                    sys.stdout.write(f"\rVelocidad Normal: {velocidad}%        ")
                sys.stdout.flush()
                # Actualizar inmediatamente la velocidad de los motores
                actualizar_pwm()

        # 2. Lógica de enviar el comando a Tiva con el botón físico
        if GPIO.input(boton) == GPIO.LOW:
            sys.stdout.write("\rBoton presionado, enviando a Tiva!")
            sys.stdout.flush()
            if ser is not None:
                ser.write(b'buzzer\n')
            time.sleep(0.3)
            
        time.sleep(0.05)

except KeyboardInterrupt:
    pass
finally:
    # Restaurar la configuración de la consola
    termios.tcsetattr(sys.stdin, termios.TCSADRAIN, old_settings)
    detener()
    pwm_m1.stop()
    pwm_m2.stop()
    GPIO.cleanup()
    if ser is not None:
        ser.close()
    print("\nPrograma finalizado de manera segura.")
