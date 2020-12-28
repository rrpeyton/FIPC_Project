#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Dec 23 20:18:37 2020
@author: rrpeyton
"""

import threading
import time
    
class FIPC_Controler:
    def __init__(self):
        self.__axis = []
        self.__axis.append(_Axis(1,"MOX_02_30"))
        self.__axis.append(_Axis(2,"MOX_02_30"))
        self.__axis.append(_Axis(3,"MOX_02_30"))
        self.__axis.append(_Axis(4,"MOR_100_30"))
        self.__axis.append(_Axis(5,"MOG_65_10"))
        self.__axis.append(_Axis(6,"MOG_65_15"))
        self.__number_max_of_command = 16
        self.__axis_number = 6
        self.__print = False

    def setPrintInfo(self, iPrint = True):        
        self.__print = iPrint
        for ii in range(self.__axis_number):
            self.__axis[ii].setPrintInfo(iPrint)
        
    def sendData(self, text):
        if self.__print:
            print("** Read commands (INIT) **")
        out = "";
        self.__command = []
        count = self.__getCommands(text)
        for ii in range(count):
            try:
                if self.__command[ii]=="?RA":
                    out += self.__getAllReport()
                elif self.__command[ii]=="?R":
                    ii += 1
                    out += self.__getReport(int(self.__command[ii]))
                elif self.__command[ii]=="?V":
                    ii += 1
                    out += self.__axis[int(self.__command[ii])-1].getSpeed()
                elif self.__command[ii]=="?A":
                    ii += 1
                    out += self.__axis[int(self.__command[ii])-1].getAccelerationTime()
                elif self.__command[ii]=="?P":
                    ii += 1
                    out += self.__axis[int(self.__command[ii])-1].getCurrentPosition()                    
                elif self.__command[ii]=="?M":
                    ii += 1
                    out += self.__axis[int(self.__command[ii])-1].isMoving()                    
                elif self.__command[ii]=="?S":
                    ii += 1
                    out += self.__axis[int(self.__command[ii])-1].getStatus()                    
                elif self.__command[ii]=="E":
                    self.__requestAction("ENABLE")
                elif self.__command[ii]=="D":
                    self.__requestAction("DISABLE")
                elif self.__command[ii]=="HA":
                    self.__requestAction("HOMING")
                elif self.__command[ii]=="SA":
                    self.__requestAction("STOP")
                elif self.__command[ii]=="H":
                    ii += 1
                    self.__requestAction("HOMING",int(self.__command[ii]))
                elif self.__command[ii]=="S":
                    ii += 1
                    self.__requestAction("STOP",int(self.__command[ii]))
                elif self.__command[ii]=="V":
                    ii += 2
                    self.__setSpeed(int(self.__command[ii-1]),float(self.__command[ii]))
                elif self.__command[ii]=="A":
                    ii += 2
                    self.__setAccelerationTime(int(self.__command[ii-1]),float(self.__command[ii]))
                elif self.__command[ii]=="MR":
                    ii += 2
                    self.__requestAction("MOVE_RELATIVE",int(self.__command[ii-1]),float(self.__command[ii]))
                elif self.__command[ii]=="MA":
                    ii += 2
                    self.__requestAction("MOVE_ABSOLUTE",int(self.__command[ii-1]),float(self.__command[ii]))
                elif self.__command[ii]=="SYNCR":
                    iDist = []
                    for jj in range(self.__axis_number):
                        ii += 1
                        iDist.append(float(self.__command[ii]))
                    ii += 2
                    self.__syncMotionRel(iDist, float(self.__command[ii-1]), float(self.__command[ii]))
                elif self.__command[ii]=="SYNCA":
                    iAbsolute = []
                    for jj in range(self.__axis_number):
                        ii += 1
                        iAbsolute.append(float(self.__command[ii]))
                    ii += 2
                    self.__syncMotionAbs(iAbsolute, float(self.__command[ii-1]), float(self.__command[ii]))
            except:
                print("Error en el comando")
                return "Error"            
        time.sleep(0.1) 
        if self.__print:
            print("** Read commands (END) **\n")                
            
        return out
    
    def __requestAction(self, iAction="NOTHING", id = -1, iData = 0.0):
        for ii in range(self.__axis_number):
            if id==ii+1 or id==-1:
                self.__axis[ii].setAction(iAction,iData)

    def __setSpeed(self, id = -1, iData = 0.0):
        for ii in range(self.__axis_number):
            if id==ii+1 or id==-1:
                self.__axis[ii].setSpeed(iData)

    def __setAccelerationTime(self, id = -1, iData = 0.0):
        for ii in range(self.__axis_number):
            if id==ii+1 or id==-1:
                self.__axis[ii].setAccelerationTime(iData)
    
    def __getCommands(self,text):
        myTrimText = text        
        for count in range(self.__number_max_of_command):
            try:
                auxIndex = myTrimText.index(':')
                self.__command.append(myTrimText[0:auxIndex])
                myTrimText = myTrimText[auxIndex+1:]
            except ValueError:
                break;
        return count

    def __getAllReport(self):
        report = ""
        for ii in range(self.__axis_number):
            report += self.__getReport(ii+1) + "\n"
        return report
        
    def __getReport(self, id):
        return self.__axis[id-1].getReport()

    def __syncMotionRel(self, iDist, iTimeSpeed, iAccelTime):
        for ii in range(self.__axis_number):
            if iDist[ii] and (not self.__axis[ii].canMoveRelative(iDist[ii])):
                return
        for ii in range(self.__axis_number):
            if iDist[ii] and (not self.__axis[ii].setSpeed(abs(iDist[ii])/iTimeSpeed)):
                return
        for ii in range(self.__axis_number):
            if iDist[ii] and (not self.__axis[ii].setAccelerationTime(iAccelTime)):
                return
        for ii in range(self.__axis_number):
            if iDist[ii]:
                self.__axis[ii].setAction("MOVE_RELATIVE",iDist[ii])

    def __syncMotionAbs(self, iAbsolute, iTimeSpeed, iAccelTime):
        iDist = []
        for ii in range(self.__axis_number):
            value = iAbsolute[ii]-float(self.__axis[ii].getCurrentPosition())
            iDist.append(value)
        self.__syncMotionRel(iDist, iTimeSpeed, iAccelTime)
            
    
class _Axis:    
    # Variables virtuales
    __targetPosition = 0.0
    __currentPosition = 0.0
    __setZero = 0.0
    
    def __init__(self, id, iType):
        self.__id = id
        self.__axis_status = "STATUS_DISABLE"
        self.__setMotorStage(iType)
        self.__print = False
        self.__thread_stop = threading.Event()
        self.__thread_moving = threading.Thread(target=self.__moving, args=(self.__thread_stop,))
        
    def setPrintInfo(self, iPrint = True):
        self.__print = iPrint
    
    def __setMotorStage(self, iType):
        if iType == "MOX_02_30":
            self.__type = iType;
            self.__veloMax = 1875
            self.__minPosition = 0
            self.__maxPosition = 30000
            self.__speed = self.__veloMax*0.2
            self.__accelTime = 1.0
            self.__units = "um"
        elif iType == "MOR_100_30":
            self.__type = iType;
            self.__veloMax = 3750
            self.__minPosition = 0
            self.__maxPosition = 360000
            self.__speed = self.__veloMax*0.2
            self.__accelTime = 1.0
            self.__units = "mgrad"
        elif iType == "MOG_65_10":
            self.__type = iType;
            self.__veloMax = 960
            self.__minPosition = -15000
            self.__maxPosition = 15000
            self.__speed = self.__veloMax*0.2
            self.__accelTime = 1.0
            self.__setZero = -15000
            self.__units = "mgrad"
        elif iType == "MOG_65_15":
            self.__type = iType;
            self.__veloMax = 1350
            self.__minPosition = -21000
            self.__maxPosition = 21000
            self.__speed = self.__veloMax*0.2
            self.__accelTime = 1.0
            self.__setZero = -21000
            self.__units = "mgrad"

    def setAction(self, iAction = "NOTHING",  iData = 0.0):
        out = False        
        if self.__axis_status=="STATUS_DISABLE":            
            if iAction=="ENABLE":
                if self.__print:
                    print("--> Enable #" + str(self.__id))    
                self.__axis_status = "STATUS_NO_HOME"
                out = True
        elif self.__axis_status=="STATUS_NO_HOME":
            if iAction=="DISABLE":
                if self.__print:
                    print("--> Disable #" + str(self.__id))    
                self.__axis_status = "STATUS_DISABLE"
                out = True
            elif iAction=="HOMING":
                if self.__print:
                    print("--> GoHome #" + str(self.__id))                    
                self.__currentPosition = self.__setZero
                self.__axis_status = "STATUS_READY"
                out = True
        elif self.__axis_status=="STATUS_READY":
            if iAction=="DISABLE":
                if self.__print:
                    print("--> Disable #" + str(self.__id))                    
                self.__axis_status = "STATUS_DISABLE"
                out = True
            elif iAction=="MOVE_RELATIVE" and self.__configMoveRelative(iData):
                if self.__print:
                    print("--> Go Relative #" + str(self.__id) + " " + str(iData))
                self.__thread_moving = threading.Thread(target=self.__moving, args=())
                self.__thread_moving.start()
                self.__axis_status = "STATUS_MOVING"
                out = True
            elif iAction=="MOVE_ABSOLUTE" and self.__configMoveAbsolute(iData):
                if self.__print:
                    print("--> Go Absolute #" + str(self.__id) + " " + str(iData))                
                self.__thread_moving = threading.Thread(target=self.__moving, args=())                    
                self.__thread_moving.start()
                self.__axis_status = "STATUS_MOVING"
                out = True
        elif self.__axis_status=="STATUS_MOVING":
            if iAction=="STOP":
                if self.__print:
                    print("--> Stop #" + str(self.__id))   
                if self.__thread_moving.is_alive():
                    self.__thread_stop.set()
                out = True               
        return out

    def __moving(self):
        Ts = 0.1
        total_time = abs(self.__targetPosition-self.__currentPosition)/self.__speed
        steps = Ts*self.__speed
        number_of_steps = int(total_time/Ts)
        for ii in range(number_of_steps):
            if self.__thread_stop.is_set():
                self.__axis_status = "STATUS_READY"
                return                
            self.__currentPosition += steps
            if self.__print:
                print("--> --> Position #" + str(self.__id) + " " + str(self.__currentPosition))
            time.sleep(Ts)
        self.__currentPosition = self.__targetPosition
        if self.__print:
            print("--> --> Position #" + str(self.__id) + " " + str(self.__currentPosition))
        self.__axis_status = "STATUS_READY"



    def isMoving(self):
        if self.__axis_status == "STATUS_MOVING":
            return "1"
        return "0"

    def getSpeed(self):
        return str(self.__speed)

    def getAccelerationTime(self):
        return str(self.__accelTime)

    def getCurrentPosition(self):
        return str(self.__currentPosition)


    def setSpeed(self, iSpeed):
        if self.__axis_status!="STATUS_READY":
            return False
        if iSpeed>0.0 and iSpeed<self.__veloMax:
            if self.__print:
                print("--> Set Speed #"+ str(self.__id) + " " + str(iSpeed) + self.__units +"/s")
            self.__speed = iSpeed
            return True
        return False

    def setAccelerationTime(self, iAccelTime):
        if self.__axis_status!="STATUS_READY":
            return False
        if iAccelTime>0.0:
            if self.__print:
                print("--> Set Accel #"+ str(self.__id) + " " + str(iAccelTime) + "seg")
            self.__accelTime = iAccelTime;
            return True
        return False;

    def canMoveRelative(self, iRelative):
        return self.canMoveAbsolute(iRelative+self.__currentPosition)

    def canMoveAbsolute(self, iAbsolute):
        if (iAbsolute<self.__minPosition) or(iAbsolute>self.__maxPosition):
            return False
        return True
    
    def getReport(self):
        if self.__print:
            print("--> Generate report #" + str(self.__id))        
        str_out = "#"+str(self.__id)
        str_out += ";" + self.getStatus();
        str_out += ";" + str(self.__currentPosition)
        str_out += ";" + self.__units
        return str_out
    
    def getStatus(self):
        str_out = ""
        if self.__axis_status == "STATUS_DISABLE":
            str_out += "Disable"
        elif self.__axis_status == "STATUS_NO_HOME":
            str_out += "NoHome"
        elif self.__axis_status == "STATUS_HOMING":
            str_out += "Homing"
        elif self.__axis_status == "STATUS_READY":
            str_out += "Ready"
        elif self.__axis_status == "STATUS_MOVING":
            str_out += "Moving"        
        return str_out

    def __configMoveRelative(self, iRelative):
        return self.__configMoveAbsolute(iRelative+self.__currentPosition)

    def __configMoveAbsolute(self, iAbsolute):
        if not self.canMoveAbsolute(iAbsolute):
            return False
        self.__targetPosition = iAbsolute # Configura movimiento en DRIVER
        # Configura velocidad en DRIVER
        # Configura aceleracion en DRIVER
        return True;    