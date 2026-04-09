# 1. Suma de n números
print("--- 1. Suma de n números ---")
n = input("¿Cuántos números deseas sumar?: ") # Pedimos la cantidad de números
n = int(n)                                    # Convertimos el texto a número entero
suma_total = 0                                # Creamos una variable que empieza en cero

for i in range(n):                            # Repetimos el proceso "n" veces
    numero = input("Ingresa un número: ")     # Pedimos el número al usuario
    numero = float(numero)                    # Convertimos a decimal por si tiene comas
    suma_total = suma_total + numero          # Vamos sumando el número al total

print("El resultado de la suma es:", suma_total) # Mostramos el resultado en pantalla
print("")                                     # Imprimimos un espacio en blanco


# 2. Invertir números (ej 619 -> 916)
print("--- 2. Invertir números ---")
numero_texto = input("Ingresa un número: ")   # Pedimos el número, se guarda como texto
numero_invertido = ""                         # Creamos un texto vacío para guardar el resultado

for digito in numero_texto:                   # Revisamos el número dígito por dígito
    numero_invertido = digito + numero_invertido # Ponemos el nuevo dígito antes de los demás

print("El número invertido es:", numero_invertido) # Mostramos el resultado
print("")                                     # Imprimimos un espacio en blanco


# 3. Mensaje Personalizado
print("--- 3. Mensaje Personalizado ---")
nombre = input("Ingresa tu nombre: ")         # Guardamos el nombre
edad = input("Ingresa tu edad: ")             # Guardamos la edad
profesion = input("Ingresa tu profesión: ")   # Guardamos la profesión

# Juntamos las palabras y variables usando el signo de suma (+)
mensaje = "Hola " + nombre + ", tienes " + edad + " años y trabajas como " + profesion + "."
print(mensaje)                                # Mostramos el mensaje en pantalla
print("")                                     # Imprimimos un espacio en blanco


# 4. Valores Únicos
print("--- 4. Valores Únicos ---")
valores = input("Ingresa números con espacios: ") # Pedimos los números todos juntos
lista_numeros = valores.split()               # Cortamos el texto por los espacios para hacer una lista
valores_unicos = []                           # Creamos una lista vacía para guardar los que no se repiten

for numero in lista_numeros:                  # Revisamos cada número de la lista original
    if numero not in valores_unicos:          # Preguntamos: ¿Este número NO está en mi nueva lista?
        valores_unicos.append(numero)         # Si no está, lo agregamos a la nueva lista

print("Los valores únicos son:", valores_unicos) # Mostramos la lista final sin repetidos