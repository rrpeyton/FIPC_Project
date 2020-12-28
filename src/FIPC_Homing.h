/*! \file FIPC_Homing.h
 *  \brief Clase que implementa la búsqueda de la referencia cero.
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

#ifndef FIPC_Homing_h
#define FIPC_Homing_h

#include "Arduino.h"
#include <AccelStepper.h>

//!  Clase que implementa la búsqueda de la referencia cero.
/*!
 * Ejecuta la secuencia de búsqueda de la referencia:
 * Si no detecta el switch mueve de forma rápida hasta detectarlo.
 * Una vez encontrada la referencia, cambia el sentido de giro y se
 * desplaza hasta que el switch combie de estado.
 */
class FIPC_Homing
{
  public:    
  
    //! Symbolic names for home status.
    typedef enum{ HOMING_NOT,   /*!< Sin referencia, en espera de la orden de inicio. */
                  HOMING_INIT,  /*!< Inicializa la secuencia. */
                  HOMING_FAST,  /*!< Se desplaza rápido hasta encontrar referencia. */
                  HOMING_SLOW,  /*!< Se desplaza lento hasta definir la referencia. */
                  HOMING_OK     /*!< Referencia encontrada. */
                  }HomingStatus;

    //! Constructor.
    /*!
      \param Puntero al driver del motor paso a paso.
      \param Entrada digital de la referencia.
     */
    FIPC_Homing(AccelStepper* pAxis, uint8_t iSwitchRef);

    //! Ejecuta la búsqueda de la referencia cero.
    /*!
      \return true la referencia fue encontrada.
    */    
    bool run();
    
    //! Detiene la búsqueda de la referencia.
    void stop();

    //! Configuración de las velocidades de desplazamiento
    /*!
      \param speedFast Máxima velocidad de desplazamiento en pasos por segundos.
      \param speedSlow Mínima velocidad de desplazamiento en pasos por segundos.
    */    
    void setSpeed(float speedFast, float speedSlow);
    
    //! Configuración de la distancia virtual al cero.
    /*!
      \param iDistance Distancia al cero en unidades de pasos.
    */    
    void setZero(long iDistance);

    //! Configuración de entrada digital.
    /*!
      \param iSwitchRef GPIO de la entrada digital.
    */    
    void setSwitch(uint8_t iSwitchRef);

    //! Retorna la entrada digital configurada.
    /*!
      \return GPIO de la entrada digital.
    */    
    uint8_t getSwitch();

    //! Retorna el estado en que se encuentra el objeto.
    /*!
      \return La variable simbólica que describe el estado del objeto.
    */    
    uint8_t getStatus();

    //! Invierte la dirección de la búsqueda.
    void    invertDirection();
  
  private:
    AccelStepper* _axis; /*!< Puntero a driver del motor paso a paso. */

    HomingStatus _status = HOMING_NOT; /*!< Almacena el estado del objeto. */

    uint8_t _switchRef; /*!< Almacena la entrada digital configurada. */

    bool _direction = true; /*!< Dirección de búsqueda de referencia. */

    long _time_saved = 0; /*!< Variable de tiempo en microsegundo para la implementación del antirebote. */    
    
    float _speedFast=0; /*!< Almacena la velocidad máxima en pasos/s. */    

    float _speedSlow=0; /*!< Almacena la velocidad mínima en pasos/s. */    

    long _absoluteZero = 0; /*!< Distancia virtual al cero en pasos. */    
};
#endif 
