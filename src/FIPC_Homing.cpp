/*! \file FIPC_Homing.cpp
    \brief Clase que implementa la búsqueda de la referencia cero.
*/

#include "FIPC_Homing.h"

#define DELAY_MS_AR 25 /*!< Tiempo de retardo para el antirebote en milisegundos. */

// Constructor.
FIPC_Homing::FIPC_Homing(AccelStepper* pAxis, uint8_t iSwitchRef) {
  _axis = pAxis;
  _switchRef = iSwitchRef;
}

// Ejecuta la búsqueda de la referencia
bool FIPC_Homing::run(){
   /* Decomentar sección si se quiere simular */
   _status = HOMING_OK; 
   _axis->setCurrentPosition(_absoluteZero);
   return false; 
   /* Decomentar sección si se quiere simular */

  switch(_status){
    case HOMING_NOT:
      _status = HOMING_INIT;
      break;
      
    case HOMING_INIT:
      _axis->setMaxSpeed(_speedFast);
    
      if( digitalRead(_switchRef) ){
        _axis->setSpeed(_speedFast);
        _status = HOMING_FAST;       
      } else {
        _axis->setSpeed(_speedSlow); 
        _status = HOMING_SLOW;          
        _time_saved = millis();
      }
      break;
      
    case HOMING_FAST:
      if( !digitalRead(_switchRef) ){ 
        _axis->setSpeed(_speedSlow);          
        _status = HOMING_SLOW;
        _time_saved = millis();
      } else {
        _axis->runSpeed(); 
      }
      break;

    case HOMING_SLOW:
      if( ((millis()-_time_saved)>DELAY_MS_AR)&&(digitalRead(_switchRef)) ){
        _axis->setCurrentPosition(_absoluteZero);
        _status = HOMING_OK;        
      } else {
        _axis->runSpeed(); 
      }
      break;

    case HOMING_OK:
      return false;
      break;
  }
  return true;
}

// Detiene y deshabilita la búsqueda de la referencia.
void FIPC_Homing::stop(){
  switch(_status){
    case HOMING_FAST: _status = HOMING_NOT; break;
    case HOMING_SLOW: _status = HOMING_NOT; break;
    case HOMING_OK:   _status = HOMING_NOT; break;
  }    
}

// Configuración de velocidades.
void FIPC_Homing::setSpeed(float speedFast, float speedSlow){
  if(_direction){
    _speedFast = -speedFast;
    _speedSlow = speedSlow;    
  } else {
    _speedFast = speedFast;
    _speedSlow = -speedSlow;    
  }
}

//Configuración de la distancia virtual al cero.
void FIPC_Homing::setZero(long iDistance){ _absoluteZero = iDistance;}

//Configuración de entrada digital.
void FIPC_Homing::setSwitch(uint8_t iSwitchRef){ _switchRef = iSwitchRef;}

//Retorna la entrada digital configurada.
uint8_t FIPC_Homing::getSwitch(){ return _switchRef;}

//Retorna el estado en que se encuentra el objeto.
uint8_t FIPC_Homing::getStatus(){ return _status;}

//Invierte la dirección de la búsqueda.
void FIPC_Homing::invertDirection(){
  if(_direction) _direction = false;
  else _direction = true;
  FIPC_Homing::setSpeed(_speedFast,_speedSlow);
}
