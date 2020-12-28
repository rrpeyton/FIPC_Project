/*! \file FIPC_Axis.h
 *  \brief Clase que implementa el control de un eje.
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

#ifndef FIPC_Axis_h
#define FIPC_Axis_h

#include "Arduino.h"
#include "FIPC_Homing.h"
#include <AccelStepper.h>

//!  Clase que implementa el control de un eje.
/*!
 *   Permite ejecutar desplazamientos absolutos o relativos con
 *   aceleración en cada tipo de eje configurado (ver tipos de 
 *   ejes permitidos FIPC_Axis::MotorStage).
 *   Además se comunica directamente con la librería 
 *   <a target="_blank" rel="noopener noreferrer" href="http://www.airspayce.com/mikem/arduino/AccelStepper">AccelStepper</a> y
 *   contiene un objeto FIPC_Homing que implementa la búsqueda del cero.
 *   
 *  
 *   \par Operación
 *   Este módulo ofrece una interfaz que le permite al usuario solicitar acciones, 
 *   mientras un proceso que se ejecuta en tiempo real actualiza el estado del motor.
 *   Una vez instanciado este objeto, antes de solicitar cualquier acción, se deberá 
 *   configurar el tipo de eje. Adicionalmente, se implementan funciones para configurar la 
 *   velocidad y aceleración de los desplazamientos, como así también una serie de 
 *   funciones de consulta.
 *   
 *   \par Acciones permitidas:
 *   \li FIPC_Axis::ACTION_ENABLE Habilita los movimientos de los motores, es decir, los energiza.
 *   \li FIPC_Axis::ACTION_HOMING Búsqueda de la posición cero del eje.
 *   \li FIPC_Axis::ACTION_MOVE_ABSOLUTE Desplazamiento en coordenadas absolutas.
 *   \li FIPC_Axis::ACTION_MOVE_RELATIVE Desplazamiento en coordenadas relativas.
 *   \li FIPC_Axis::ACTION_STOP Detiene cualquier desplazamiento.
 *   \li FIPC_Axis::ACTION_DISABLE Deshabilita los movimientos de los motores, es decir, los desenergiza.
 *  
 *   \par Estados del eje:
 *   La implementación se basa en una máquina de estados que describe el estado del eje.
 *   El valor se guarda en la variable _axis_status y puede ser consultada por el usuario 
 *   con la función getStatus(). Los estados posibles son:
 *   \li FIPC_Axis::STATUS_DISABLE Los ejes están deshabilitados y se pierde la referencia.
 *   \li FIPC_Axis::STATUS_NO_HOME Estado al que llega luego de habilitarse el eje. En este estado solo se permite buscar el cero.
 *   \li FIPC_Axis::STATUS_HOMING El eje se encuentra desplazándose hacia la referencia.
 *   \li FIPC_Axis::STATUS_READY Una vez que el eje encontró la referencia o terminó un movimiento, el eje queda en espera por una acción.
 *   \li FIPC_Axis::STATUS_MOVING El eje se encuentra desplazándose.
 *   
 *   \par Tipos de ejes implementados
 *   Este proyecto está basado en una plataforma de 6 ejes de la empresa optics-focus 
 *   ( https://www.optics-focus.com/6axis-motorized-positioning-stage-p-661.html), sin embargo
 *   podría agregarse el tipo de eje que se desee. Los posibles ejes implementados en este proyecto son:
 *   \li FIPC_Axis::MOX_02_30  para más info https://www.optics-focus.com/miniature-motorized-linear-stage-p-569.html
 *   \li FIPC_Axis::MOR_100_30 para más info https://www.optics-focus.com/motorized-rotation-stage-p-523.html
 *   \li FIPC_Axis::MOG_65_10  para más info https://www.optics-focus.com/motorized-goniometer-stage-p-534.html
 *   \li FIPC_Axis::MOG_65_15  para más info https://www.optics-focus.com/motorized-goniometer-stage-p-535.html
 *  
 *   \par Advertencias
 *   En cada tipo de eje se preconfigura los límites de posición máximos y mínimos, 
 *   como así también la velocidad máxima basada en mediciones en el límite de generación
 *   de pulsos. La frecuencia máxima utilizada es 12 kHz.
 *   Si el usuario solicita una acción que no está permitida, será rechazada. 
 *   
 *   Para el correcto funcionamiento el usuario debe garantizar que la función exec() sea
 *   ejecutada con una frecuencia mayor a 12 kHz, mientras se está ejecutando un desplazamiento. 
*/
class FIPC_Axis {
  public:

    //! Definicion de variable simbólica de acciones
    /*!
     * Utilizar como parámetro cuando se utiliza setAction()
     */    
    typedef enum {ACTION_NOTHING,         /*!< No hace nada. */
                  ACTION_STOP,            /*!< Detener un desplazamiento. */
                  ACTION_ENABLE,          /*!< Habilitar o energizar. */
                  ACTION_DISABLE,         /*!< Deshabilitar o desenergizar. */
                  ACTION_HOMING,          /*!< Ejecuta la búsqueda del cero. */
                  ACTION_MOVE_ABSOLUTE,   /*!< Deplazamiento en coordenadas absolutas. */
                  ACTION_MOVE_RELATIVE    /*!< Deplazamiento en coordenadas relativas. */
                  } AxisAction;

    //! Definicion de variable simbólica de tipos de ejes
    /*!
     * Utilizar como parámetro cuando se utiliza setMotorStage()
     */    
    typedef enum {MOX_02_30,    /*!< https://www.optics-focus.com/miniature-motorized-linear-stage-p-569.html */
                  MOR_100_30,   /*!< https://www.optics-focus.com/motorized-rotation-stage-p-523.html */
                  MOG_65_10,    /*!< https://www.optics-focus.com/motorized-goniometer-stage-p-534.html */
                  MOG_65_15     /*!< https://www.optics-focus.com/motorized-goniometer-stage-p-535.html */
                  } MotorStage;


    //! Constructor.
    /*!
     * Los parámetros necesarios corresponden a la configuración de hardware.
     * \param set_id     Identificador del eje.
     * \param pinSTEP    GPIO del pin de pulsos del Driver8825.
     * \param pinDIR     GPIO del pin de dirección del Driver8825.
     * \param pinEN      GPIO del pin de habilitación del Driver8825.
     * \param switch_1   GPIO del pin del switch de positivo.
     * \param switch_2   GPIO del pin del switch de negativo.
     * \param switch_ref GPIO del pin del switch de referencia.
     */
    FIPC_Axis(uint8_t set_id, uint8_t pinSTEP, uint8_t pinDIR, uint8_t pinEN, uint8_t switch_1, uint8_t switch_2, uint8_t switch_ref);
    
    //! Establece el tipo de eje.
    /*!
     * \param type Variable simbólica de tipo de eje.
    */    
    void setMotorStage(MotorStage type);
    
    //! Verifica si puede ejecutar un desplazamiento en coordenadas relativas.
    /*!
     * Las unidades dependen del tipo de eje. Para ejes lineales se utiliza micrómetros,
     * mientras que para ejes angulares se utiliza miligrados.
     * 
     * \param iRelative Destino en coordenadas relativas deseado.
     * \return true si el desplazamiento puede realizarse.
    */    
    bool canMoveRelative(float iRelative = 0.0);
    
    //! Verifica si puede ejecutar un desplazamiento en coordenadas absolutas.
    /*!
     * Las unidades dependen del tipo de eje. Para ejes lineales se utiliza micrómetros,
     * mientras que para ejes angulares se utiliza miligrados.
     * 
     * \param iRelative Destino en coordenadas absolutas deseado.
     * \return true si el desplazamiento puede realizarse.
    */    
    bool canMoveAbsolute(float iAbsolute = 0.0);
    
    //! Configura la velocidad de desplazamiento.
    /*!
     * Las unidades dependen del tipo de eje. Para ejes lineales se 
     * utiliza micrómetros por segundo, mientras que para ejes angulares 
     * se utiliza miligrados por segundo.
     * 
     * \param iSpeed La velocidad deseada.
     * \return true si la velocidad se configuró correctamente.
    */    
    bool setSpeed(float iSpeed);

    //! Configura el tiempo de aceleración del desplazamiento.
    /*!
     * Las unidades del tiempo es en segundos.
     * 
     * \param iAccelTime Tiempo de aceleración.
     * \return true si el tiempo de aceleración se configuró correctamente.
    */    
    bool setAccelerationTime(float iAccelTime);
        
    //! Solicita una acción.
    /*!
     * \param iAction Tipo de acción solicitada.
     * \param iData Parámetro adicional de acciónm por ejemplo desplazamiento.
     * \return false si no se realizará ninguna acción.
    */    
    bool setAction(uint8_t iAction = FIPC_Axis::ACTION_NOTHING, float iData = 0.0);
        
    //! Ejecuta el control de los motores.
    /*!
     * Esta función deberá ser llamada recurrentemente en tiempo real.
    */    
    void exec();

    //! Solicita un reporte general del objeto.
    /*!
     * \return Una cadena de caracteres que describe el estado completo del objeto.
    */    
    String getReport();

    //! Solicita un reporte del estado del objeto.
    /*!
     * \return Una cadena de caracteres con el estado en que se encuentra el objeto.
    */    
    String getStatus();
        
    //! Solicita la velocidad configurada.
    /*!
     * \return Una cadena de caracteres con la velocidad configurada.
    */    
    String getSpeed();

    //! Solicita el tiempo de aceleración configurado.
    /*!
     * \return Una cadena de caracteres con el tiempo de aceleración configurado.
    */    
    String getAccelerationTime();

    //! Solicita la posición actual en coordenadas absolutas.
    /*!
     * \return Una cadena de caracteres la posición actual del eje.
    */    
    String getCurrentPosition();

    //! Verifica si el eje se está moviendo.
    /*!
     * \return "1" si se está moviendo.
    */    
    String isRunning();

  private:
    //! Definicion de variable simbólica interna de estado del motor.
    typedef enum {STATUS_DISABLE, /*!< Eje deshabilitado. */
                  STATUS_NO_HOME, /*!< Eje habilitado y sin referencia de cero. */
                  STATUS_HOMING,  /*!< Desplazándose en búsqueda de la referencia cero. */
                  STATUS_READY,   /*!< Eje habilitado y en espera por un desplazamiento. */
                  STATUS_MOVING   /*!< Desplazándose. */
                  } AxisStatus;
                  
    //! Definicion de variable simbólica interna de tipos de ejecución.
    typedef enum {EXEC_WAIT,          /*!< En espera por una ejecución. */
                  EXEC_STOP,          /*!< Debe ejecutar una parada. */
                  EXEC_RUN,           /*!< Debe ejecutar un desplazamiento. */
                  EXEC_HOMING,        /*!< Debe ejecutar la búsqueda de la referencia cero. */
                  EXEC_HOMING_STOP,   /*!< Debe ejecutar una parada de la búsqueda de la referencia cero. */
                  EXEC_ENABLE,        /*!< Debe habilitar el eje.  */
                  EXEC_DISABLE        /*!< Debe deshabilitar el eje. */
                  } ExecAccelStepper;

    AccelStepper* _Axis; /*!< Puntero al driver del motor paso a paso. */
    
    FIPC_Homing*  _Homing; /*!< Puntero al objeto encargado de realizar la búsqueda de la referencia cero. */

    AxisStatus _axis_status  = STATUS_DISABLE;  /*!< Almacena el estado del eje. */

    ExecAccelStepper  _newExec = EXEC_WAIT; /*!< Almacena el tipo de ejecución. */

    MotorStage _type; /*!< Almacena el tipo de eje configurado. */

    uint8_t _id; /*!< Identificador. */
    
    String _units; /*!< Tipo de unidad configurada. */
    
    bool _direction; /*!< Sentido de giro. */

    float _speed; /*!< Velocidad configurada. */

    float _accelTime; /*!< Tiempo de aceleración configurado. */
  
    uint8_t _switch_1; /*!< GPIO del switch de límite positivo. */

    uint8_t _switch_2; /*!< GPIO del switch de límite negativo. */

    uint8_t _switch_ref; /*!< GPIO del switch de referencia. */
    
    float _factorToStep; /*!< Factor de conversión en [step/unidad]. */
   
    float _minPosition; /*!< Posicion mínima absoluta. */
    
    float _maxPosition; /*!< Posicion máxima absoluta. */
    
    float _veloMax; /*!< Velocidad máxima permitida. */

    //! Invierte la dirección de desplazamiento.
    void invertDirection();

    //! Configura el destino en coordenadas absolutas.
    /*!
     * \param iAbsolute Destino en coordenadas absolutas.
     * \return true si la configuración del destino fue satisfactorio.
    */    
    bool configMoveAbsolute(float iAbsolute);
    
    //! Configura el destino en coordenadas relativas.
    /*!
     * \param iAbsolute Destino en coordenadas relativas.
     * \return true si la configuración del destino fue satisfactorio.
    */    
    bool configMoveRelative(float iRelative);
};

#endif 
