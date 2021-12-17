# -*- coding: utf-8 -*-
"""
Created on Fri Dec 17 10:18:24 2021

@author: FISilicio
"""

import serial       # libreria pyserial

class FIPC_controler:
    def __init__(self, name='FIPC_controler'):
        self.name = name
        self.__serial = serial.Serial()
        self.config_serial()
    
    def config_serial(self, port='COM3', baudrate=115200, timeout=0.5):
        self.__serial.port = port
        self.__serial.baudrate = baudrate
        self.__serial.timeout = timeout


    def open(self, port='COM3', baudrate=115200, timeout=None):
        try:
            self.__serial.open()
            if self.__serial.is_open:
                print("Controlador conectado")
        except ValueError:
            print("Error de conección")

    def close(self, port='COM3', baudrate=115200, timeout=None):
        if self.__serial.is_open:
            self.__serial.close()

    def send(self, data):
        self.__serial.write(data.encode('utf-8'))
    
    def ask(self, command):
        self.send(command)
        
        data = self.__serial.readline()
        while data.decode('utf-8')=='':
            data = self.__serial.readline()
           
        out_str = ''    
        while data.decode('utf-8')!='\n':
            out_str = out_str+data.decode('utf-8')
            data = self.__serial.readline()
            if not len(data):
                break
        self.__serial.readline()
        return out_str


    
    
    
    