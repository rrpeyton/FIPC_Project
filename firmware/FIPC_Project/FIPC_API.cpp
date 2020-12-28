/*! \file FIPC_API.cpp
 *  \brief Interfaz de aplicación del controlador.
 *  \include FIPC_API.cpp
*/

#include "FIPC_API.h"

#define NUMBER_MAX_OF_COMMAND 16 /*!< Numero de comando de lectura. */


// Constructor.
FIPC_API::FIPC_API(){
  // Crea ejes
  _axis[0] = new FIPC_Axis(1, STEP_01, DIR_01, EN, SW1_01, SW2_01, SW2_01);
  _axis[1] = new FIPC_Axis(2, STEP_02, DIR_02, EN, SW1_02, SW2_02, SW2_02);
  _axis[2] = new FIPC_Axis(3, STEP_03, DIR_03, EN, SW1_03, SW2_03, SW2_03);
  _axis[3] = new FIPC_Axis(4, STEP_04, DIR_04, EN, SW1_04, SW2_04, SW2_04);
  _axis[4] = new FIPC_Axis(5, STEP_05, DIR_05, EN, SW1_05, SW2_05, SW2_05);
  _axis[5] = new FIPC_Axis(6, STEP_06, DIR_06, EN, SW1_06, SW2_06, SW2_06);

  // Configura los ejes
  _axis[0]->setMotorStage(FIPC_Axis::MOX_02_30);
  _axis[1]->setMotorStage(FIPC_Axis::MOX_02_30);
  _axis[2]->setMotorStage(FIPC_Axis::MOX_02_30);
  _axis[3]->setMotorStage(FIPC_Axis::MOR_100_30);
  _axis[4]->setMotorStage(FIPC_Axis::MOG_65_10);
  _axis[5]->setMotorStage(FIPC_Axis::MOG_65_15);
}


// Proceso de ejecución en tiempo real
void FIPC_API::exec(void* pvParameters){
  _axis[0]->exec(); _axis[1]->exec(); _axis[2]->exec();
  _axis[3]->exec(); _axis[4]->exec(); _axis[5]->exec();
}

    
/******************************************/ 
/* Begin: Public                          */

// Método público de interfaz con la aplicación.
String FIPC_API::request(String myString){
  String out = "";
  String command[NUMBER_MAX_OF_COMMAND];
  int8_t count;

  // lectura de comandos
  count = FIPC_API::getCommands(myString,command);
  
  for(uint8_t i = 0; i<count; i++){
    if( command[i].equals(API_Q_REPO_ALL))  out += getAllReport()+"\n"; 
    if( command[i].equals(API_Q_REPO))      out += getReport(command[++i].toInt())+"\n";
    if( command[i].equals(API_Q_STAT))      out += getStatus(command[++i].toInt())+"\n";
    if( command[i].equals(API_Q_ISMOV))     out += isRunning(command[++i].toInt())+"\n";    
    if( command[i].equals(API_Q_POS))       out += getCurrentPosition(command[++i].toInt())+"\n";
    if( command[i].equals(API_Q_VELO))      out += getSpeed(command[++i].toInt())+"\n";
    if( command[i].equals(API_Q_ACCEL))     out += getAccelerationTime(command[++i].toInt())+"\n"; 
    if( command[i].equals(API_ENABLE))      requestAction(FIPC_Axis::ACTION_ENABLE); 
    if( command[i].equals(API_DISABLE))     requestAction(FIPC_Axis::ACTION_DISABLE); 
    if( command[i].equals(API_HOME_ALL))    requestAction(FIPC_Axis::ACTION_HOMING); 
    if( command[i].equals(API_STOP_ALL))    requestAction(FIPC_Axis::ACTION_STOP); 
    if( command[i].equals(API_HOME))        requestAction(FIPC_Axis::ACTION_HOMING,command[++i].toInt());
    if( command[i].equals(API_STOP))        requestAction(FIPC_Axis::ACTION_STOP,command[++i].toInt());
    if( command[i].equals(API_VELO))        setSpeed(command[++i].toInt(),command[++i].toFloat());
    if( command[i].equals(API_ACCEL))       setAccelerationTime(command[++i].toInt(),command[++i].toFloat());
    if( command[i].equals(API_RELATIVE))    requestAction(FIPC_Axis::ACTION_MOVE_RELATIVE,command[++i].toInt(),command[++i].toFloat());
    if( command[i].equals(API_ABSOLUTE))    requestAction(FIPC_Axis::ACTION_MOVE_ABSOLUTE,command[++i].toInt(),command[++i].toFloat());

    // get Sync motion
    if( command[i].equals(API_SYNC_REL) ){
      float iDist[AXIS_NUMBERS];
      for(uint8_t j = 0; j<AXIS_NUMBERS; j++) iDist[j] = command[++i].toFloat();
      FIPC_API::syncMotionRel(iDist, command[++i].toFloat(), command[++i].toFloat());
    }

    // get Sync motion
    if( command[i].equals(API_SYNC_ABS) ){
      float iAbsolute[AXIS_NUMBERS];
      for(uint8_t j = 0; j<AXIS_NUMBERS; j++) iAbsolute[j] = command[++i].toFloat();
      FIPC_API::syncMotionAbs(iAbsolute, command[++i].toFloat(), command[++i].toFloat());
    }

  }// END FOR

  return out;
}

/* End: Public                            */
/******************************************/ 



/******************************************/ 
/* Begin: Private                         */

// Intérprete de comandos
int8_t FIPC_API::getCommands(String myString, String command[]){
  String myTrimString = myString;
  int8_t auxIndex, count;
  
  for(count=0; count<NUMBER_MAX_OF_COMMAND; count++){
    auxIndex = myTrimString.indexOf(':');
    if( auxIndex==-1 ) break; // check if exist other command
    command[count] = myTrimString.substring(0,auxIndex);
    myTrimString = myTrimString.substring(auxIndex+1);    
  }
  
  return count;    
}

// Solicitud de acciones a los ejes
void FIPC_API::requestAction(uint8_t action, int8_t id, float iData){  
  for (uint8_t i = 0; i<AXIS_NUMBERS; i++) // if (id = -1) same request to all axis
    if( (id==i+1)||(id==-1) ) _axis[i]->setAction(action,iData);
}       

// Configuración de velocidad
void FIPC_API::setSpeed(int8_t id, float iData){  
  for (uint8_t i = 0; i<AXIS_NUMBERS; i++) // if (id = -1) same to all axis
    if( (id==i+1)||(id==-1) ) _axis[i]->setSpeed(iData);
}       

// Configuración de aceleración
void FIPC_API::setAccelerationTime(int8_t id, float iData){  
  for (uint8_t i = 0; i<AXIS_NUMBERS; i++) // if (id = -1) same to all axis
    if( (id==i+1)||(id==-1) ) _axis[i]->setAccelerationTime(iData);
}       

// Genera una solicitud de movimiento sincrónico en coordenadas relativas.
void FIPC_API::syncMotionRel(float iDist[],float iTimeSpeed, float iAccelTime){  
  // Primero verifica que todos los desplazamiento puedan realizarse
  // y luego realiza la solicitud a cada eje
  uint8_t i;
  for (i = 0; i<AXIS_NUMBERS; i++) // check if can move that distance    
    if( (iDist[i])&&(!_axis[i]->canMoveRelative(iDist[i])) ) return;  
    
  for (i = 0; i<AXIS_NUMBERS; i++) // check and config speeds
    if( (iDist[i])&&(!_axis[i]->setSpeed(abs(iDist[i])/iTimeSpeed)) ) return;  

  for (i = 0; i<AXIS_NUMBERS; i++) // check and config acceleration times
    if( (iDist[i])&&(!_axis[i]->setAccelerationTime(iAccelTime)) ) return;  

  for (i = 0; i<AXIS_NUMBERS; i++) // if all config were accepeted, then request action
    if( iDist[i] ) _axis[i]->setAction(FIPC_Axis::ACTION_MOVE_RELATIVE,iDist[i]);    
}

// Genera una solicitud de movimiento sincrónico en coordenadas absolutas.
void FIPC_API::syncMotionAbs(float iAbsolute[],float iTimeSpeed, float iAccelTime){  
  // Primero debe calcular la distancia y llama a la funcion
  // de movimiento sincrónico
  float iDist[AXIS_NUMBERS];
  for (uint8_t i = 0; i<AXIS_NUMBERS; i++) // check if can move that distance    
    iDist[i] = iAbsolute[i]-(_axis[i]->getCurrentPosition()).toFloat();

  FIPC_API::syncMotionRel(iDist,iTimeSpeed, iAccelTime);  
}

// Retorna un reporte del estado de un eje.
String FIPC_API::getReport(uint8_t id){
  return _axis[id-1]->getReport();  
}

// Retorna un reporte completo.
String FIPC_API::getAllReport(){
  String report = "";
  for(uint8_t i = 0; i<AXIS_NUMBERS; i++)
    report = report + FIPC_API::getReport(i+1)+"\n";
  return report;  
}

// Retorna la velocidad configurada de un eje.
String FIPC_API::getSpeed(uint8_t id){
  return _axis[id-1]->getSpeed();  
}

// Retorna el tiempo de aceleración configurado de un eje.
String FIPC_API::getAccelerationTime(uint8_t id){
  return _axis[id-1]->getAccelerationTime();  
}

// Retorna la posición absoluta de un eje.
String FIPC_API::getCurrentPosition(uint8_t id){
  return _axis[id-1]->getCurrentPosition();  
}

// Verifica si un eje se está moviendo.
String FIPC_API::isRunning(uint8_t id){
  return _axis[id-1]->isRunning();  
}

// Retorna el estado de un eje.
String FIPC_API::getStatus(uint8_t id){
  return _axis[id-1]->getStatus();  
}

/* End: Private                           */
/******************************************/ 
