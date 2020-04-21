/**
  ******************************************************************************
  * @file    stm32h7xx_it.c
  * @author  MCD Application Team
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_it.h"
#include "main.h"

#if defined(USE_USB_FS) || defined(USE_USB_HS)
extern HCD_HandleTypeDef hhcd;
#endif /* USE_USB_FS | USE_USB_HS */

//extern UART_HandleTypeDef 	UART_Handle1;

void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(BUTTON_WAKEUP_PIN);
}

#if defined(USE_JOYSTICK)
/**
  * @brief  This function handles External line 2 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI2_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(SEL_JOY_PIN);
}

/**
  * @brief  This function handles External line 3 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI3_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(DOWN_JOY_PIN);
}

/**
  * @brief  This function handles External line 4 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI4_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(LEFT_JOY_PIN);
}
#endif /* USE_JOYSTICK */

/**
  * @brief  This function handles External lines 9 to 5 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{
#if defined(USE_JOYSTICK)
  HAL_GPIO_EXTI_IRQHandler(RIGHT_JOY_PIN);
  HAL_GPIO_EXTI_IRQHandler(UP_JOY_PIN);
#endif /* USE_JOYSTICK */
#if defined(USE_SDCARD)
  	  HAL_GPIO_EXTI_IRQHandler(SD_DETECT_PIN);
#endif /* USE_SDCARD */
  	  HAL_GPIO_EXTI_IRQHandler(TS_INT_PIN);
}

#if defined(USE_SDCARD)
/**
* @brief  This function handles SDMMC interrupt request.
* @param  None
* @retval None
*/
void SDMMC1_IRQHandler(void)
{
	BSP_SD_IRQHandler(0);
}
#endif /* USE_SDCARD */

#if defined(USE_USB_FS) || defined(USE_USB_HS)
/**
  * @brief  This function handles USB-On-The-Go FS/HS global interrupt request.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_FS
void OTG_FS_IRQHandler(void)
#else
void OTG_HS_IRQHandler(void)
#endif
{
	HAL_HCD_IRQHandler(&hhcd);
}
#endif /* USE_USB_FS | USE_USB_HS */

void HSEM1_IRQHandler(void)
{
	HAL_HSEM_IRQHandler();
}

