/*! \file FIPC_Axis.cpp
    \brief Clase que implementa el control de un eje.
*/

#include "FIPC_Axis.h"

# define INIT_FACTOR_SPEED  0.2   /*!< Factor de velocidad máxima configurada al asignar tipo de eje. */
# define INIT_ACCEL_TIME    1.0   /*!< Identificador. */
# define HOME_FACTOR_SLOW   0.02  /*!< Factor de velocidad máxima configurada al asignar tipo de eje en búsqueda de cero lenta. */
# define HOME_FACTOR_FAST   0.1   /*!< Factor de velocidad máxima configurada al asignar tipo de eje en búsqueda de cero rápida. */

// Constructor.
FIPC_Axis::FIPC_Axis(uint8_t set_id, uint8_t pinSTEP, uint8_t pinDIR, uint8_t pinEN, uint8_t switch_1, uint8_t switch_2, uint8_t switch_ref) {
  _id = set_id;

  // Inicializa y configura una nueva instancia de AccelStepper
  _direction = false;
  _Axis = new AccelStepper(AccelStepper::DRIVER, pinSTEP, pinDIR);    
  _Axis->setEnablePin(pinEN);
  _Axis->setPinsInverted(_direction,false,true);
  _Axis->disableOutputs();

  // Configura las GPIO de entrada
  pinMode(_switch_1 = switch_1, INPUT);
  pinMode(_switch_2 = switch_2, INPUT);

  // Construye una nueva instancia de la clase FIPC_Homing
  _Homing = new FIPC_Homing(_Axis,_switch_ref=switch_ref); 
}


/******************************************/ 
/* Begin: Public                          */


// Configuración de los tipos de ejes.
// Está basado en los datasheet de los ejes
// para más información ver https://www.optics-focus.com/6axis-motorized-positioning-stage-p-661.html.
// La configuración de la máxima velocidad corresponde a 6000 pasos per second.
void FIPC_Axis::setMotorStage(MotorStage type){
  switch(type){
    case MOX_02_30:
      _type = type;
      _factorToStep = 3.2;      // en [_units/um]
      _veloMax = 1875;          // en [_units/s]
      _minPosition = 0;         // en [_units]
      _maxPosition = 30000;     // en [_units]
      _speed = _veloMax*INIT_FACTOR_SPEED;
      _accelTime = INIT_ACCEL_TIME;
      _units = String("um");
      break;
      
    case MOR_100_30:
      _type = type;
      _factorToStep = 1.6;      // en [step/_units]
      _veloMax = 3750;          // en [_units/s]
      _minPosition = 0;         // en [_units]
      _maxPosition = 360000;    // en [_units]
      _speed = _veloMax*INIT_FACTOR_SPEED; 
      _accelTime = INIT_ACCEL_TIME;
      FIPC_Axis::invertDirection();
      _units = String("mgrad");
      break;
      
    case MOG_65_10:
      _type = type;
      _factorToStep = 6.25;     // en [step/_units]
      _veloMax = 960;          // en [_units/s]
      _minPosition = -15000;    // en [_units]
      _maxPosition = 15000;     // en [_units]
      _speed = _veloMax*INIT_FACTOR_SPEED; 
      _accelTime = INIT_ACCEL_TIME;
      _Homing->setZero(-15000*_factorToStep);
      _units = String("mgrad");
      break;
      
    case MOG_65_15:
      _type = type;
      _factorToStep = 4.44444;  // en [step/_units]
      _veloMax = 1350;          // en [_units/s]
      _minPosition = -21000;    // en [_units]
      _maxPosition = 21000;     // en [_units]
      _speed = _veloMax*INIT_FACTOR_SPEED;
      _accelTime = INIT_ACCEL_TIME;
      _Homing->setZero(-21000*_factorToStep);
      _units = String("mgrad");
      break;
  }

  _Homing->setSpeed(HOME_FACTOR_FAST*_veloMax*_factorToStep, HOME_FACTOR_SLOW*_veloMax*_factorToStep);
}

// Analiza la acción según el estado en que se encuentra el objeto
bool FIPC_Axis::setAction(uint8_t iAction, float iData){
  switch(_axis_status) {
    case STATUS_DISABLE:
      if( iAction==ACTION_ENABLE ) {
        _newExec = EXEC_ENABLE;
        return true;
      }
      break;
    case STATUS_NO_HOME:
      if( iAction==ACTION_DISABLE ){
        _newExec = EXEC_DISABLE;
        return true;
      }
      if( iAction==ACTION_HOMING ){
        _newExec = EXEC_HOMING;
        return true;
      }
      break;
    case STATUS_HOMING:
      if( iAction==ACTION_STOP ){
        _newExec = EXEC_STOP;
        return true;
      }
      break;
    case STATUS_READY:
      if( iAction==ACTION_DISABLE ){
        _newExec = EXEC_DISABLE;
        return true;
      }
      if( ((iAction==ACTION_MOVE_RELATIVE)&&(FIPC_Axis::configMoveRelative(iData))) ||
          ((iAction==ACTION_MOVE_ABSOLUTE)&&(FIPC_Axis::configMoveAbsolute(iData))) ){
            _newExec = EXEC_RUN;
            return true;            
          }
      break;
    case STATUS_MOVING:
      if( iAction==ACTION_STOP ){
        _newExec = EXEC_STOP;
        return true;
      }
      break;        
  }
  return false;
}

// Configuración de velocidad
bool FIPC_Axis::setSpeed(float iSpeed){
  if( _axis_status!=STATUS_READY ) return false;
  if( (iSpeed>0.0)&&(iSpeed<_veloMax) ) {
    _speed = iSpeed;
    return true;
  }
  return false;
}

// Configuración de aceleración
bool FIPC_Axis::setAccelerationTime(float iAccelTime){  
  if( _axis_status!=STATUS_READY ) return false;
  if( iAccelTime>0.0 ) {
    _accelTime = iAccelTime;
    return true;
  }
  return false;
}


// Verifica si puede realizar el desplazamiento (coordenadas relativas)
bool FIPC_Axis::canMoveRelative(float iRelative){    
  return FIPC_Axis::canMoveAbsolute(iRelative+_Axis->currentPosition()/_factorToStep);
}

// Verifica si puede realizar el desplazamiento (coordenadas absolutas)
bool FIPC_Axis::canMoveAbsolute(float iAbsolute){  
  if( (iAbsolute<_minPosition)||(iAbsolute>_maxPosition) ) return false;
  return true;
}

// Elabora un reporte completo del estado del objeto
String FIPC_Axis::getReport(){ 
  String str_out = "#"+String(_id);   
  str_out += ";" + FIPC_Axis::getStatus();
  str_out += ";" + String(_Axis->currentPosition()/_factorToStep,2);
  str_out += ";" + _units;
  return str_out;
}

// Retorna el estado en que se encuentra el objeto
String FIPC_Axis::getStatus(){ 
  String str_out = "";   
  switch(_axis_status){
    case STATUS_DISABLE: str_out +=  "Disable"; break;  
    case STATUS_NO_HOME: str_out +=  "NoHome"; break;  
    case STATUS_HOMING:  str_out +=  "Homing"; break;  
    case STATUS_READY:   str_out +=  "Ready";  break;  
    case STATUS_MOVING:  str_out +=  "Moving"; break;  
  }
  return str_out;
}

// Retorna la velocidad configurada.
String FIPC_Axis::getSpeed(){
  return String(_speed,2);
}

// Retorna el tiempo de aceleración configurado.
String FIPC_Axis::getAccelerationTime(){
  return String(_accelTime,2);
}

// Retorna la posición actual en coordenadas absolutas.
String FIPC_Axis::getCurrentPosition(){
  return String(_Axis->currentPosition()/_factorToStep,2);
}

// Retorna verificación de movimiento.
String FIPC_Axis::isRunning(){
  if(_Axis->isRunning())  return "1";
  else  return "0";
}

/*------------ PROCESO EN TIEMPO REAL ----------*/
void FIPC_Axis::exec(){
  // 1° debe atender si se está desplazando
  if( _axis_status==STATUS_MOVING ) {
    if( _newExec==EXEC_STOP ) {
      _Axis->stop();
      _newExec = EXEC_WAIT;
    }
    
    if( !_Axis->run() ) _axis_status = STATUS_READY;
    return;
  }
  
  // 2° Debe atender si se está buscando el cero
  if( _axis_status==STATUS_HOMING ) {
    if( _newExec==EXEC_STOP ) {
      _Homing->stop();
      _newExec = EXEC_WAIT;
      _axis_status = STATUS_NO_HOME;
      return;
    }
    if( !_Homing->run() ) _axis_status = STATUS_READY;
    return;
  }

  // 3° Espera por una acción de desplazamiento
  if( _axis_status==STATUS_READY ) {    
    if( _newExec==EXEC_RUN ) {  
      _axis_status = STATUS_MOVING;          
      _newExec = EXEC_WAIT;
    }
    if( _newExec==EXEC_DISABLE ) { 
      _Axis->disableOutputs();
      _Homing->stop();
      _axis_status = STATUS_DISABLE;
      _newExec = EXEC_WAIT;
    }    
    return;
  }      

  // 4° espera para buscar el cero
  if( _axis_status==STATUS_NO_HOME ) {
    if( _newExec==EXEC_HOMING ) {
      _axis_status = STATUS_HOMING;
      _newExec = EXEC_WAIT;
    }
    if( _newExec==EXEC_DISABLE ) { 
      _Axis->disableOutputs();
      _Homing->stop();
      _axis_status = STATUS_DISABLE;
      _newExec = EXEC_WAIT;
    }    
    return;
  }      

  // 5° espera para ser habilitado
  if( _axis_status==STATUS_DISABLE ) {
    if( _newExec==EXEC_ENABLE ) {
      _Axis->enableOutputs();
      _axis_status = STATUS_NO_HOME;
      _newExec = EXEC_WAIT;
    }    
    return;
  }      
}
/*------------ PROCESO EN TIEMPO REAL ----------*/
/* End: Public                            */
/******************************************/ 


/******************************************/ 
/* Begin: Private                         */

// Invierte el sentido de giro
void FIPC_Axis::invertDirection(){
  _Axis->setPinsInverted(_direction=!_direction,false,true);
}

// Configura un desplazamiento en coordenadas relativas
bool FIPC_Axis::configMoveRelative(float iRelative){
  return FIPC_Axis::configMoveAbsolute(iRelative + _Axis->currentPosition()/_factorToStep);  
}

// Configura un desplazamiento en coordenadas absolutas
bool FIPC_Axis::configMoveAbsolute(float iAbsolute){
  if( !FIPC_Axis::canMoveAbsolute(iAbsolute) )  return false;      
  
  _Axis->moveTo(iAbsolute*_factorToStep);
  _Axis->setMaxSpeed(_speed*_factorToStep);
  _Axis->setAcceleration(_speed*_factorToStep/_accelTime);  
  return true;
}
/* End: Private                           */
/******************************************/ 
