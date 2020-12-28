/*! \file FIPC_Project.ino
 *  \brief Programa principal del proyecto.
*/

/*!  
 *  \mainpage FIPC_Project
 * 
 *  Controlador de una plataforma motorizada de 6 ejes
 *  basado en el uso de la librería AccelStepper comanda la generación de pulsos
 *  utilizando drivers DRV8825. Para acceder a la documentación de la librería 
 *  AccelStepper ir a la página http://www.airspayce.com/mikem/arduino/AccelStepper
 * 
 *  La implementación está realizada en una placa de desarrollo ESP32 NodeMCU (Node32s)
 *  y se utiliza como sistema operativo en tiempo real FreeRTOS.
 *  
 *  Para más información sobre la lista de comandos implementados en la API ver \ref API_Commands.
 * 
 *  El proyecto fue desarrollado por el grupo de investigación de Fotónica Integrada
 *  perteneciente al Centro de Investigaciones Ópticas, Argentina.
 *  
 *  \par Diagrama de estado de los ejes
 *  
 *  \image html status_diagram.png
 * 
 *  \par Historia de versiones
 *  \version v1.0 Versión inicial
 * 
 *  \par Copyright
 * 
 *  This software is Copyright (C) 2020-2021 Roberto Peyton. Use is subject to license
 *  conditions. The licensing is GPL V3.
 * 
 *  This is the appropriate option if you want to share the source code of your
 *  application with everyone you distribute it to, and you also want to give them
 *  the right to share who uses it. If you wish to use this software under Open
 *  Source Licensing, you must contribute all your source code to the open source
 *  community in accordance with the GPL Version 23 when your application is
 *  distributed. See https://www.gnu.org/licenses/gpl-3.0.html
 * 
 *  \author  Roberto Peyton (robertop@ciop.unlp.edu.ar)
 *  Copyright (C) 2020-2021 Roberto Peyton
*/

#include "FIPC_API.h"

#define EXEC_TIME_OUT 80 // exec time-out in microseconds
unsigned long dt_exec,t1_exec,flag_time_out=0; // global variable time analysis

FIPC_API axis_api;

void TaskReadAction   ( void *pvParameters ); // execute in core 0
void TaskReportStatus ( void *pvParameters ); // execute in core 0
void TaskExec         ( void *pvParameters ); // execute in core 1
SemaphoreHandle_t xSerialSemaphore;

void setup() {
  Serial.begin(115200);

  // config FreeRTOS
  if ( xSerialSemaphore==NULL ) {
    xSerialSemaphore = xSemaphoreCreateMutex();
    if ( xSerialSemaphore!=NULL ) xSemaphoreGive( xSerialSemaphore );
  }
  xTaskCreatePinnedToCore(TaskReadAction,"TaskReadAction",3*1024,NULL,2,NULL,0);
  xTaskCreatePinnedToCore(TaskReportStatus,"TaskReportStatus",4*1024,NULL,2,NULL,0);    
  xTaskCreatePinnedToCore(TaskExec,"TaskExec",2*1024,NULL,configMAX_PRIORITIES-1,NULL,1);
}

/****************** CORE 0 ******************/
/********************************************/
// loop task delete
void loop() { vTaskDelete(NULL); }

// Real time execute task
void TaskExec(void *pvParameters) {
  (void) pvParameters;
  t1_exec = micros();
  for (;;) { 
    
    axis_api.exec(pvParameters);

    // check execution time out
    if( (dt_exec=micros()-t1_exec)>EXEC_TIME_OUT ) flag_time_out = dt_exec;
    t1_exec = micros();
  }
}
/********************************************/
/****************** CORE 0 ******************/


/****************** CORE 1 ******************/
/********************************************/
// Tarea de reporte de estado
void TaskReportStatus(void *pvParameters) {
  (void) pvParameters;
  for (;;) {
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ){
      
      Serial.println(axis_api.request("?RA:"));
      
      Serial.println("**** "+String(dt_exec)+"us ****");
      if(flag_time_out) {
        Serial.println("##### Time out "+String(flag_time_out)+"us");
        flag_time_out = 0;
      }
      Serial.println("");
      
      xSemaphoreGive( xSerialSemaphore );
    }
    vTaskDelay(2000);  // one tick delay (100ms) in between reads for stability
  }
}

// Tarea de lectura de comandos
void TaskReadAction(void *pvParameters) {
  (void) pvParameters;
  String str_out = "";
  for (;;) {
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ){
      if (Serial.available() > 0) {
        str_out = axis_api.request( Serial.readStringUntil('\n') );
        if( str_out!="" ) Serial.print(str_out);
      }
      xSemaphoreGive( xSerialSemaphore );
    }    
    vTaskDelay(100);
  }
}
/********************************************/
/****************** CORE 1 ******************/
