#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Dec 24 10:40:04 2020
@author: rrpeyton
"""


from FIPC_Controler import FIPC_Controler
import time


usb0 = FIPC_Controler()

#####################################################
# comando para habilitar la impresion de lo que sucede con los ejes
#####################################################
usb0.setPrintInfo(False) # True activa la impresion



#####################################################
# Comandos que hacen una accion... Algunos ejemplos
#####################################################
print("\n...")
print("Ejemplo de comandos iniciales")
usb0.sendData("E:")  # habilita todos los ejes
usb0.sendData("H:1:") # busca la referencia del eje #1

usb0.sendData("D:")  # deshabilita todos los ejes
usb0.sendData("E:")  # habilita todos los ejes
usb0.sendData("HA:") # busca la referencia de todos los ejes


#####################################################
# Comandos que ejecutan un movimiento... Algunos ejemplos
#####################################################
# las acciones de movimiento ejecutan un Thread!!

print("\n...")
# Ejecuta un movimiento relativo del eje #2
print("Ejecuta un movimiento, espera 2.5 segundos y lo detiene")
usb0.sendData("MR:2:10856.3:") # las acciones de movimiento ejecutan un Thread!!
# espera 2.5 segundos y detiene el movimiento
time.sleep(2.5)
usb0.sendData("S:2:")
print(usb0.sendData("?R:2:"))

print("\n...")
# Configura la velocidad, la aceleracion y ejecuta un movimiento absoluto del eje #5
print("Espera que termine movimiento (1)... " + usb0.sendData("?R:5:"))
usb0.sendData("V:5:650:A:5:1.5:MA:5:-5000:") 
# Espera a que finalice el movimiento
while( usb0.sendData("?M:5:")=="1" ):# "1" se mueve, "0" finalizo el movimiento
    print("Espera que termine movimiento (1)... " + usb0.sendData("?R:5:"))
    time.sleep(1)
    pass 

print("\n...")
# Ejecuta un movimiento absoluto sincronico.
print("Espera que termine movimiento (2)... "+ usb0.sendData("?R:6:"))
usb0.sendData("SYNCA:0:0:0:0:0:0:20:0.5:") 
while( usb0.sendData("?M:6:")=="1" ):# "1" se mueve, "0" finalizo el movimiento
    print("Espera que termine movimiento (2)... "+ usb0.sendData("?R:6:"))
    time.sleep(1)
    pass 

print("\n...")
# # Ejecuta un movimiento relativo sincronico. Ejes #1, #3 y #6
print("Espera que termine movimiento (3)... "+ usb0.sendData("?R:1:"))
usb0.sendData("SYNCR:100:0:40.5:0:0:1000.6:6:0.5:") 
while( usb0.sendData("?M:1:")=="1" ):# "1" se mueve, "0" finalizo el movimiento
    print("Espera que termine movimiento (3)... "+ usb0.sendData("?R:1:"))
    time.sleep(1)
    pass 

# #####################################################
# # Comandos que piden información... Algunos ejemplos
# #####################################################
print("\n...")
print("##################################\n")
print("Ejemplos de pedido de informacion:")
read_usb0 = usb0.sendData("?RA:") # reporte completo
print("Reporte completo \n"+read_usb0)

read_usb0 = usb0.sendData("?R:3:") # reporte del eje 3
print("\nReporte solamente del #3: \n"+read_usb0)

read_usb0 = usb0.sendData("?V:1:") # pide la velocidad del eje 1
print("\nSolicita velocidad del eje #1: \n"+read_usb0)

read_usb0 = usb0.sendData("?A:5:") # pide el tiempo de aceleracion del eje 5
print("\nSolicita tiempo de aceleración del eje #5: \n"+read_usb0)

read_usb0 = usb0.sendData("?P:4:") # pide la posicion absoluta del eje 4
print("\nSolicita posición absoluta del eje #4: \n"+read_usb0)

read_usb0 = usb0.sendData("?S:6:") # pide el estado del eje 6
print("\nSolicita el estado del eje #6: \n"+read_usb0)
