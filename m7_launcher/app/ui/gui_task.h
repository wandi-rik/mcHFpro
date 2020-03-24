/**
  ******************************************************************************
  * @file    MenuLauncher.h
  * @author  MCD Application Team
  * @brief   headr file for MenuLauncher.c module 
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#ifndef __GUI_TASK_H
#define __GUI_TASK_H

/*********************************************************************
*
*       Includes
*
**********************************************************************
*/
#include <stdlib.h>

#include "GUI.h"

/*********************************************************************
*
*       Exported constants
*
**********************************************************************
*/
#ifndef GUI_CONST_STORAGE
  #define GUI_CONST_STORAGE const
#endif

#ifdef GUI_TASK

/*********************************************************************
*
*       Exported variables
*
**********************************************************************
*/
typedef union
{
  struct
  {
    uint32_t     A1 : 15;
    uint32_t     B1 : 16;
    uint32_t     Reserved : 1;
  }b;
  uint32_t d32;
}CALIBRATION_Data1Typedef;

typedef union
{
  struct
  {
    uint32_t      A2 : 15;
    uint32_t      B2 : 16;
    uint32_t      IsCalibrated : 1;
  }b;
  uint32_t d32;

} CALIBRATION_Data2Typedef;

//void ml_periodic(void);
void gui_task(void const *arg);

#endif

#endif  /* Avoid multiple inclusion */
/*************************** End of file ****************************/
