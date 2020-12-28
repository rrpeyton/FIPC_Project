/*! \file FIPC_API.h
 *  \brief Interfaz de aplicación del controlador.
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

#ifndef FIPC_API_h
#define FIPC_API_h

#include "Arduino.h"
#include "FIPC_pinTable.h"
#include "FIPC_Axis.h"

#define AXIS_NUMBERS 6    /*!< Cantidad de ejes. */

/**
 * \defgroup API_Commands Comandos de API
 * 
 * Lista de comandos interpretados por la API, ver algunos ejemplo a continuación.
 * 
 * Es necesario que todo los comandos enviados a request() terminen con el caracter <b>':'</b>.
 * 
 * \par Ejemplos:
 * 
 * \li <b>"E:"</b> Se habilitan los ejes.
 * \li <b>"HA:"</b> Busca la referencia de todos los ejes al mismo tiempo.
 * \li <b>"H:1:H:3:H:6:"</b> Busca la referencia de los ejes número #1, #3 y #6.
 * \li <b>"V:4:550:A:4:0.5:MR:4:1340.5:"</b> Configura la velocidad del eje #4 en 550 mgrad/s, 
 * el tiempo de aceleración en 0.5 segundos y por último ejecuta un desplazamiento relativo de 1340.5 mgrad.
 * \li <b>"SYNCR:45:31.5:0:0:0:15.6:2.5:0.1:"</b> Ejecuta un movimiento sincrónico en coordenadas
 * relativas. El anteúltimo elemento define el tiempo del desplazamiento (2.5 segundos) mientras que 
 * el último elemento define el tiempo de aceleración (0.1 segundos). Notar que los ejes #3, #4 y #5 no
 * realizarán movimientos.
 * 
 * @{
 */
#define API_ENABLE     "E"     /*!< Habilita el sistema. */
#define API_DISABLE    "D"     /*!< Deshabilita el sistema. */
#define API_HOME_ALL   "HA"    /*!< Busca la referencia de todos los ejes. */
#define API_STOP_ALL   "SA"    /*!< Detiene todos los ejes. */
#define API_HOME       "H"     /*!< Busca referencia de un eje. */
#define API_STOP       "S"     /*!< Detiene un eje. */
#define API_VELO       "V"     /*!< Configura la velocidad de 1 eje. */
#define API_ACCEL      "A"     /*!< Configura el tiempo de aceleración de 1 eje. */
#define API_RELATIVE   "MR"    /*!< Configura el desplazamiento relativo de 1 eje. */
#define API_ABSOLUTE   "MA"    /*!< Configura el desplazamiento absoluto de 1 eje. */
#define API_SYNC_REL   "SYNCR" /*!< Configura un desplazamiento syncrónico en coordenadas relativas. */
#define API_SYNC_ABS   "SYNCA" /*!< Configura un desplazamiento syncrónico en coordenadas absolutas. */

#define API_Q_REPO_ALL "?RA"   /*!< Solicitud. Reporte de todos los ejes. */
#define API_Q_REPO     "?R"    /*!< Solicitud. Reporte de 1 eje. */
#define API_Q_STAT     "?S"    /*!< Solicitud. Retorna el estado en el que se encuentra 1 eje. */
#define API_Q_ISMOV    "?M"    /*!< Solicitud. Para verificar si 1 eje se está moviendo ("1" se está moviendo). */
#define API_Q_POS      "?P"    /*!< Solicitud. Retorna la posición en condenadas absolutas de 1 eje. */
#define API_Q_VELO     "?V"    /*!< Solicitud. Retorna la velocidad configurada de 1 eje. */
#define API_Q_ACCEL    "?A"    /*!< Solicitud. Retorna el tiempo de aceleración configurado de 1 eje. */
/**@}*/




//!  Clase que implementa una interfaz de aplicación para controlar 6 ejes.
/*!
 *   La API está formada por la función exec() que debe ser llamada en un proceso 
 *   a ejecutarse en tiempo real y por método que interpreta comandos a ejecutar
 *   llamada request(). Para más información sobre los comandos (ver \ref API_Commands).
*/
class FIPC_API{
  public:    
    //! Constructor.
    /*!
     *  Al instanciar la API se crean los 6 ejes.
     */ 
    FIPC_API();
    
    //! Proceso a ejecutar en tiempo real.
    /*!
     *  Es aconsejable ejecutar esta función lo más rápido posible.
     */ 
    void exec(void* pvParameters);
    
    //! Método público de interfaz con la aplicación.
    /*!
     *  \param myString Texto con una lista de comandos.
     *  \return Texto con el reporte solicitado.
     */     
    String request(String myString);        
    
  private:
    FIPC_Axis *_axis[AXIS_NUMBERS]; /*!< Lista de ejes. */

    //! Lectura de comandos solicitados
    /*!
     *  \param myString Texto completo de comandos.
     *  \param iTimeSpeed Vector de comandos donde guardar la lectura.
     *  \return iAccelTime Cantidad de comandos interpretados.
     */         
    int8_t getCommands(String myString, String command[]);

    //! Solicita una acción al eje.
    /*!
     *  \param action Acción a ejecutar.
     *  \param id Identificador del eje.
     *  \param iData Valor a pasar como acción.
     */     
    void requestAction(uint8_t action = 0, int8_t id = -1, float iData = 0.0);

    //! Configura el tiempo de aceleración.
    /*!
     *  \param id Identificador del eje.
     *  \param iData Valor a configurar.
     */     
    void setAccelerationTime(int8_t id, float iData);

    //! Configura la velocidad.
    /*!
     *  \param id Identificador del eje.
     *  \param iData Valor a configurar.
     */     
    void setSpeed(int8_t id, float iData);

    //! Genera una solicitud de movimiento sincrónico en coordenadas relativas.
    /*!
     *  \param iDist Vector con las distancias relativas del desplazamiento.
     *  \param iTimeSpeed Tiempo total del desplazamiento.
     *  \param iAccelTime Tiempo de aceleración del desplazamiento.
     */     
    void syncMotionRel(float iDist[],float iTimeSpeed, float iAccelTime);

    //! Genera una solicitud de movimiento sincrónico en coordenadas absolutas.
    /*!
     *  \param iAbsolute Vector con las coordenadas absolutas.
     *  \param iTimeSpeed Tiempo total del desplazamiento.
     *  \param iAccelTime Tiempo de aceleración del desplazamiento.
     */     
    void syncMotionAbs(float iAbsolute[],float iTimeSpeed, float iAccelTime);

    //! Retorna un reporte del estado de un eje.
    /*!
     *  \param id Identificador del eje.
     *  \return Un texto con el reporte solicitado.
     */     
    String getReport(uint8_t id);

    //! Retorna un reporte completo.
    /*!
     *  \param id Identificador del eje
     *  \return Un texto con el reporte solicitado.
     */     
    String getAllReport();

    //! Retorna la velocidad configurada de un eje.
    /*!
     *  \param id Identificador del eje.
     *  \return Un texto con la velocidad.
     */     
    String getSpeed(uint8_t id);

    //! Retorna la aceleracion configurada de un eje.
    /*!
     *  \param id Identificador del eje.
     *  \return Un texto con el tiempo de aceleración.
     */     
    String getAccelerationTime(uint8_t id);

    //! Retorna la posición absoluta actual de un eje.
    /*!
     *  \param id Identificador del eje.
     *  \return Un texto con la posición absoluta del eje.
     */     
    String getCurrentPosition(uint8_t id);    

    //! Verifica si un eje se está moviendo.
    /*!
     *  \param id Identificador del eje.
     *  \return "1" si el eje se está desplazando.
     */     
    String isRunning(uint8_t id);

    //! Retorna el estado de un eje.
    /*!
     *  \param id Identificador del eje.
     *  \return Un texto con el estado en el que se encuentra el eje.
     */     
    String getStatus(uint8_t id);
};
#endif 
