#!/bin/bash

# 1. Leer el valor del archivo
FRECUENCIA=$(cat frecuencia.txt)

echo "Configurando velocidad: $FRECUENCIA segundos..."

# 2. Crear un archivo de cabecera con el valor
# Esto evita romper el Makefile
echo "#define VELOCIDAD $FRECUENCIA" > config.h

# 3. Compilar y Flashear
# Usamos el make normal para que no se pierdan las librerías
make clean
make

# Solo si el make fue exitoso, flasheamos
if [ $? -eq 0 ]; then
    sudo lm4flash gcc/project0.bin
    echo "Tiva actualizada a $FRECUENCIA s."
else
    echo "Error en la compilación. Revisa el código."
fi
