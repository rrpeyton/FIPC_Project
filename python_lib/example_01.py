# -*- coding: utf-8 -*-
"""
Created on Fri Dec 17 10:42:00 2021

@author: FISilicio
"""

from module_motion_controler import FIPC_controler
import time 

TIME_FOR_REQUEST= 2

# crea objeto controlador
fipc = FIPC_controler()

# configura el puerto
fipc.config_serial(port='COM3', baudrate=115200, timeout=0.5)

# abre el puerto
fipc.open()


# pregunta --> estado de todos los motores
print(fipc.ask('?RA:'))

# ejecuta accion --> busca home
fipc.send('E:')

# pregunta --> estado de todos los motores
print(fipc.ask('?RA:'))


# ejecuta accion --> busca home
fipc.send('HA:')
# espera a que llegue al home
while(fipc.ask('?M:1:')=='1\n'):
    time.sleep(TIME_FOR_REQUEST)
while(fipc.ask('?M:2:')=='1\n'):
    time.sleep(TIME_FOR_REQUEST)
while(fipc.ask('?M:3:')=='1\n'):
    time.sleep(TIME_FOR_REQUEST)    
while(fipc.ask('?M:4:')=='1\n'):
    time.sleep(TIME_FOR_REQUEST)    
while(fipc.ask('?M:5:')=='1\n'):
    time.sleep(TIME_FOR_REQUEST)    
while(fipc.ask('?M:6:')=='1\n'):
    time.sleep(TIME_FOR_REQUEST)    
    
    
# pregunta --> estado de todos los motores
print(fipc.ask('?RA:'))


# ejecuta accion --> busca home
fipc.send('MR:3:1000:')
# espera a que termine movimiento
while(fipc.ask('?M:3:')=='1\n'):
    time.sleep(TIME_FOR_REQUEST)    

# pregunta --> estado de todos los motores
print(fipc.ask('?R:3:'))

# cierra el puerto
fipc.close()


    
