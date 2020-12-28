/*! \file FIPC_pinTable.h
 *  \brief Definiciones de Hardware.
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

#ifndef FIPC_pinTable_h
#define FIPC_pinTable_h

#include "FIPC_Axis.h"

#define   EN    02 /*!< Output. Señal de habilitación. */

#define   DIR_01    00 /*!< Output. Señal de dirección del eje N°1. */
#define   STEP_01   15 /*!< Output. Señal de paso del eje N°1. */
#define   SW1_01    04 /*!< Input. Switch hacia coordenadas positivas del eje N°1. */
#define   SW2_01    16 /*!< Input. Switch hacia coordenadas negativas del eje N°1. */

#define   DIR_02    19 /*!< Output. Señal de dirección del eje N°2. */
#define   STEP_02   18 /*!< Output. Señal de paso del eje N°2. */
#define   SW1_02    17 /*!< Input. Switch hacia coordenadas positivas del eje N°2. */
#define   SW2_02    05 /*!< Input. Switch hacia coordenadas negativas del eje N°2. */

#define   DIR_03    13 /*!< Output. Señal de dirección del eje N°3. */
#define   STEP_03   23 /*!< Output. Señal de paso del eje N°3. */
#define   SW1_03    21 /*!< Input. Switch hacia coordenadas positivas del eje N°3. */
#define   SW2_03    22 /*!< Input. Switch hacia coordenadas negativas del eje N°3. */

#define   DIR_04    32 /*!< Output. Señal de dirección del eje N°4. */
#define   STEP_04   33 /*!< Output. Señal de paso del eje N°4. */
#define   SW1_04    36 /*!< Input. Switch hacia coordenadas positivas del eje N°4. */
#define   SW2_04    36 /*!< Input. Switch hacia coordenadas negativas del eje N°4. */

#define   DIR_05    25 /*!< Output. Señal de dirección del eje N°5. */
#define   STEP_05   26 /*!< Output. Señal de paso del eje N°5. */
#define   SW1_05    34 /*!< Input. Switch hacia coordenadas positivas del eje N°5. */
#define   SW2_05    39 /*!< Input. Switch hacia coordenadas negativas del eje N°5. */

#define   DIR_06    27 /*!< Output. Señal de dirección del eje N°6. */
#define   STEP_06   12 /*!< Output. Señal de paso del eje N°6. */
#define   SW1_06    14 /*!< Input. Switch hacia coordenadas positivas del eje N°6. */
#define   SW2_06    35 /*!< Input. Switch hacia coordenadas negativas del eje N°6. */

#endif 
