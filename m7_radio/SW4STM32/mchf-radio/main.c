/**
  ******************************************************************************
  * @file    MenuLauncher/Core/CM7/Src/main.c
  * @author  MCD Application Team
  *          This is the main program for Cortex-M7
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "mchf_pro_board.h"
#include "version.h"
#include "k_rtc.h"

//#include "splash.h"
#include "bsp.h"
#include "WM.h"
//#include "gui_task.h"

#include "esp32_proc.h"
#include "ui_proc.h"
#include "icc_proc.h"
#include "audio_proc.h"
#include "touch_proc.h"

#if configAPPLICATION_ALLOCATED_HEAP == 1
#if defined ( __ICCARM__ )
#pragma location="heap_mem"
#else
__attribute__((section("heap_mem")))
#endif
uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
#endif /* configAPPLICATION_ALLOCATED_HEAP */

/* Private typedef -----------------------------------------------------------*/
typedef  void (*pFunc)(void);

#define PWR_CFG_SMPS    0xCAFECAFE
#define PWR_CFG_LDO     0x5ACAFE5A

typedef struct pwr_db
{
  __IO uint32_t t[0x30/4];
  __IO uint32_t PDR1;
}PWDDBG_TypeDef;

/* Private define ------------------------------------------------------------*/
//#define HSEM_ID_0                       (0U) /* HW semaphore 0*/
#define AUTO_DEMO_TIMEOUT_0               20
#define AUTO_DEMO_TIMEOUT_1                5

#define TS_TaskPRIORITY                 osPriorityHigh /* osPriorityNormal osPriorityRealtime */
#define TS_TaskSTACK_SIZE               (configMINIMAL_STACK_SIZE * 2)

#define AUTODEMO_TaskPRIORITY           osPriorityAboveNormal /* osPriorityNormal osPriorityRealtime */
#define AUTODEMO_TaskSTACK_SIZE         (configMINIMAL_STACK_SIZE * 2)

#define GUI_TaskPRIORITY                osPriorityNormal /* osPriorityNormal osPriorityRealtime */
#define GUI_TaskSTACK_SIZE              (configMINIMAL_STACK_SIZE * 128)

/* Private macro -------------------------------------------------------------*/
#define PWDDBG                          ((PWDDBG_TypeDef*)PWR)
#define DEVICE_IS_CUT_2_1()             (HAL_GetREVID() & 0x21ff) ? 1 : 0

/* External variables --------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//static __IO uint32_t SoftwareReset = 0;
//static uint32_t AutoDemoTimeOutMs = AUTO_DEMO_TIMEOUT_0;
//static uint32_t AutoDemoId = 0;

uint8_t BSP_Initialized = 0;

//osSemaphoreId TSSemaphoreID;
//osMessageQId AutoDemoEvent = {0};

uint32_t wakeup_pressed = 0; /* wakeup_pressed = 1 ==> User request calibration */

CALIBRATION_Data1Typedef data1;
CALIBRATION_Data2Typedef data2;
uint8_t ts_calibration_done = 0;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
//static void MPU_Config(void);
//static void CPU_CACHE_Enable(void);
//static void TouchScreenTask(void const *argument);

//extern void SUBDEMO_StartAutoDemo(const uint8_t demo_id);

extern struct	UI_DRIVER_STATE			ui_s;

TaskHandle_t hIccTask;
TaskHandle_t hTouchTask;

struct TransceiverState 	ts;

#if 0
static void TouchScreenTask(void const *argument)
{
  /* Create the TS semaphore */
  osSemaphoreDef(TSSemaphore);
  TSSemaphoreID = osSemaphoreCreate(osSemaphore(TSSemaphore), 1);

  /* initially take the TS Lock */
  osSemaphoreWait( TSSemaphoreID, osWaitForever );

  while(1)
  {
    osSemaphoreWait( TSSemaphoreID, osWaitForever );

    /* Capture input event and updade cursor */
    if(BSP_Initialized == 1)
    {
      if(BSP_TouchUpdate() && AutoDemoEvent)
      {
        osMessagePut ( AutoDemoEvent, AUTO_DEMO_RESET, 0);
      }
    }
  }
}
#endif

/**
  * @brief  EXTI line detection callbacks.
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(!BSP_Initialized)
    return;

  switch(GPIO_Pin)
  {
    case BUTTON_WAKEUP_PIN :
    {
      /* Turn LED RED off */
      //BSP_LED_Off(LED_RED);

    	printf("wakeup click\r\n");

    	if(ui_s.req_state == MODE_DESKTOP)
    		ui_s.req_state = MODE_MENU;
    	else
    		ui_s.req_state = MODE_DESKTOP;
#if 0
      if(SplashScreen_IsRunning())
      {
        wakeup_pressed = 1;
        SplashScreen_Stop();
      }
#endif
    }
    break;

#if defined(USE_SDCARD)
    case SD_DETECT_PIN :
    {
      if(!StorageEvent)
        return;

      if((BSP_SD_IsDetected(0)))
      {
        /* After sd disconnection, a SD Init is required */
        if(Storage_GetStatus(MSD_DISK_UNIT) == STORAGE_NOINIT)
        {
          if( BSP_SD_Init(0) == BSP_ERROR_NONE )
          {
            if( BSP_SD_DetectITConfig(0) == BSP_ERROR_NONE )
              osMessagePut ( StorageEvent, MSDDISK_CONNECTION_EVENT, 0);
          }
        }
        else
        {
          osMessagePut ( StorageEvent, MSDDISK_CONNECTION_EVENT, 0);
        }
      }
      else
      {
        osMessagePut ( StorageEvent, MSDDISK_DISCONNECTION_EVENT, 0);
      }
    }
    break;
#endif /* USE_SDCARD */

#if defined(USE_JOYSTICK)
    case SEL_JOY_PIN :
    {
      /* Toggle LED GREEN */
      BSP_LED_Toggle(LED_GREEN);
    }
    break;
#endif /* USE_JOYSTICK */

    case TS_INT_PIN:
    	touch_proc_irq();
    	break;

    default:
      break;
  }
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 400000000 (Cortex-M7 CPU Clock)
  *            HCLK(Hz)                       = 200000000 (Cortex-M4 CPU, Bus matrix Clocks)
  *            AHB Prescaler                  = 2
  *            D1 APB3 Prescaler              = 2 (APB3 Clock  100MHz)
  *            D2 APB1 Prescaler              = 2 (APB1 Clock  100MHz)
  *            D2 APB2 Prescaler              = 2 (APB2 Clock  100MHz)
  *            D3 APB4 Prescaler              = 2 (APB4 Clock  100MHz)
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 5
  *            PLL_N                          = 160
  *            PLL_P                          = 2
  *            PLL_Q                          = 4
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /*!< Supply configuration update enable */
#if defined(USE_PWR_LDO_SUPPLY)
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
#else
  if(DEVICE_IS_CUT_2_1() == 0)
  {
    /* WA to avoid loosing SMPS regulation in run mode */
    PWDDBG->PDR1 = 0xCAFECAFE;
    __DSB();
    PWDDBG->PDR1 |= (1<<5 | 1<<3);
    __DSB();
  }
  HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);
#endif /* USE_PWR_LDO_SUPPLY */

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 160;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;

  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    Error_Handler(7);
  }

  /* Select PLL as system clock source and configure  bus clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_PCLK1 | \
                                 RCC_CLOCKTYPE_PCLK2  | RCC_CLOCKTYPE_D3PCLK1);

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
  if(ret != HAL_OK)
  {
    Error_Handler(8);
  }

  /* Configures the External Low Speed oscillator (LSE) drive capability */
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_HIGH);

  /*##-1- Configure LSE as RTC clock source ##################################*/
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler(9);
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler(10);
  }

  /*##-2- Enable RTC peripheral Clocks #######################################*/
  /* Enable RTC Clock */
  __HAL_RCC_RTC_ENABLE();

  /*
  Note : The activation of the I/O Compensation Cell is recommended with communication  interfaces
          (GPIO, SPI, FMC, QSPI ...)  when  operating at  high frequencies(please refer to product datasheet)
          The I/O Compensation Cell activation  procedure requires :
        - The activation of the CSI clock
        - The activation of the SYSCFG clock
        - Enabling the I/O Compensation Cell : setting bit[0] of register SYSCFG_CCCSR
  */

  __HAL_RCC_CSI_ENABLE() ;

  __HAL_RCC_SYSCFG_CLK_ENABLE() ;

  HAL_EnableCompensationCell();
}

/**
  * @brief  Configure the MPU attributes as Write Through for Internal D1SRAM.
  * @note   The Base Address is 0x24000000 since this memory interface is the AXI.
  *         The Configured Region Size is 512KB because same as D1SRAM size.
  * @param  None
  * @retval None
  */
static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;

  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU attributes as WB for Flash */
  MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress      = FLASH_BASE;
  MPU_InitStruct.Size             = MPU_REGION_SIZE_2MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable     = MPU_ACCESS_BUFFERABLE;
  MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number           = MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Setup SDRAM in Write-through (framebuffer) */
  MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress      = SDRAM_DEVICE_ADDR;
  MPU_InitStruct.Size             = MPU_REGION_SIZE_32MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number           = MPU_REGION_NUMBER1;
  MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Setup AXI SRAM, SRAM1 and SRAM2 in Write-through */
  MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress      = D1_AXISRAM_BASE;
  MPU_InitStruct.Size             = MPU_REGION_SIZE_512KB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number           = MPU_REGION_NUMBER2;
  MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Setup D2 SRAM1 & SRAM2 in Write-through */
  MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress      = D2_AXISRAM_BASE;
  MPU_InitStruct.Size             = MPU_REGION_SIZE_256KB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number           = MPU_REGION_NUMBER3;
  MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Configure the MPU attributes as shareable for SRAM */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = D3_SRAM_BASE;
  MPU_InitStruct.Size = MPU_REGION_SIZE_64KB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER4;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
* @brief  CPU L1-Cache enable.
* @param  None
* @retval None
*/
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(int err)
{
  /* Turn LED RED on */
  //if(BSP_Initialized)
//	BSP_LED_On(LED_RED);

  printf( " Error Handler %d\n",err);
  configASSERT (0);
}

void BSP_ErrorHandler(void)
{
  if(BSP_Initialized)
  {
    printf( "%s(): BSP Error !!!\n", __func__ );
   // BSP_LED_On(LED_RED);
  }
}

#ifdef configUSE_MALLOC_FAILED_HOOK
/**
  * @brief  Application Malloc failure Hook
  * @param  None
  * @retval None
  */
void vApplicationMallocFailedHook(TaskHandle_t xTask, char *pcTaskName)
{
  printf( "%s(): MALLOC FAILED !!!\n", pcTaskName );

  Error_Handler(18);
}
#endif /* configUSE_MALLOC_FAILED_HOOK */

#ifdef configCHECK_FOR_STACK_OVERFLOW
void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
  printf( "%s(): STACK OVERFLOW !!!\n", pcTaskName );

  Error_Handler(19);
}
#endif /* configCHECK_FOR_STACK_OVERFLOW */

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

//*----------------------------------------------------------------------------
//* Function Name       : TransceiverStateInit
//* Object              :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void TransceiverStateInit(void)
{
	// Defaults always
	ts.txrx_mode 		= TRX_MODE_RX;				// start in RX
	ts.samp_rate		= SAI_AUDIO_FREQUENCY_48K;		// set sampling rate

	//ts.enc_one_mode 	= ENC_ONE_MODE_AUDIO_GAIN;
	//ts.enc_two_mode 	= ENC_TWO_MODE_RF_GAIN;
	//ts.enc_thr_mode		= ENC_THREE_MODE_RIT;

	ts.band		  		= BAND_MODE_20;				// band from eeprom
	ts.band_change		= 0;						// used in muting audio during band change
	ts.filter_band		= 0;						// used to indicate the bpf filter selection for power detector coefficient selection
	ts.dmod_mode 		= DEMOD_LSB;				// demodulator mode
	ts.audio_gain		= 0;//DEFAULT_AUDIO_GAIN;		// Set initial volume
	ts.audio_gain		= 0;//MAX_VOLUME_DEFAULT;		// Set max volume default
	ts.audio_gain_active = 1;						// this variable is used in the active RX audio processing function

	ts.rf_gain			= 25;//DEFAULT_RF_GAIN;			//
	ts.max_rf_gain		= 3;//MAX_RF_GAIN_DEFAULT;		// setting for maximum gain (e.g. minimum S-meter reading)
	ts.rf_codec_gain	= 9;//DEFAULT_RF_CODEC_GAIN_VAL;	// Set default RF gain (0 = lowest, 8 = highest, 9 = "Auto")
	ts.rit_value		= 0;						// RIT value
	ts.agc_mode			= 1;//AGC_DEFAULT;				// AGC setting
	ts.agc_custom_decay	= 12;//AGC_CUSTOM_DEFAULT;		// Default setting for AGC custom setting - higher = slower



	ts.filter_id		= AUDIO_DEFAULT_FILTER;		// startup audio filter
	ts.filter_300Hz_select	= FILTER_300HZ_DEFAULT;	// Select 750 Hz center filter as default
	ts.filter_500Hz_select	= FILTER_500HZ_DEFAULT;	// Select 750 Hz center filter as default
	ts.filter_1k8_select	= FILTER_1K8_DEFAULT;	// Select 1425 Hz center filter as default
	ts.filter_2k3_select	= FILTER_2K3_DEFAULT;	// Select 1412 Hz center filter as default
	ts.filter_3k6_select	= FILTER_3K6_DEFAULT;	// This is enabled by default
	ts.filter_wide_select	= FILTER_WIDE_DEFAULT;	// This is enabled by default


	//
	ts.st_gain			= 5;//DEFAULT_SIDETONE_GAIN;	// Sidetone gain
	ts.keyer_mode		= 0;//CW_MODE_IAM_B;			// CW keyer mode
	ts.keyer_speed		= 15;//DEFAULT_KEYER_SPEED;		// CW keyer speed
	ts.sidetone_freq	= 750;//CW_SIDETONE_FREQ_DEFAULT;	// CW sidetone and TX offset frequency
	ts.paddle_reverse	= 0;						// Paddle defaults to NOT reversed
	ts.cw_rx_delay		= 8;//CW_RX_DELAY_DEFAULT;		// Delay of TX->RX turnaround
	ts.unmute_delay_count		= 450;//SSB_RX_DELAY;		// Used to time TX->RX delay turnaround

	//
	ts.nb_setting		= 0;						// Noise Blanker setting
	//
	ts.tx_iq_lsb_gain_balance 	= 0;				// Default settings for RX and TX gain and phase balance
	ts.tx_iq_usb_gain_balance 	= 0;
	ts.tx_iq_lsb_gain_balance 	= 0;
	ts.tx_iq_usb_gain_balance 	= 0;
	ts.rx_iq_lsb_gain_balance = 0;
	ts.rx_iq_usb_gain_balance = 0;
	ts.rx_iq_lsb_phase_balance = 0;
	ts.rx_iq_usb_phase_balance = 0;
	//
	ts.tune_freq		= 0;
	ts.tune_freq_old	= 0;
	//
//	ts.calib_mode		= 0;						// calibrate mode
	ts.menu_mode		= 0;						// menu mode
	ts.menu_item		= 0;						// menu item selection
	ts.menu_var			= 0;						// menu item change variable
	ts.menu_var_changed	= 0;						// TRUE if a menu variable was changed and that an EEPROM save should be done

	//ts.txrx_lock		= 0;						// unlocked on start
	ts.audio_unmute		= 0;						// delayed un-mute not needed
	ts.buffer_clear		= 0;						// used on return from TX to purge the audio buffers

	ts.tx_audio_source	= 0;//TX_AUDIO_MIC;				// default source is microphone
	ts.tx_mic_gain		= 15;//MIC_GAIN_DEFAULT;			// default microphone gain
	ts.tx_mic_gain_mult	= ts.tx_mic_gain;			// actual operating value for microphone gain
	ts.mic_boost		= 0;
	ts.tx_line_gain		= 12;//LINE_GAIN_DEFAULT;		// default line gain

	ts.tune				= 0;						// reset tuning flag

	ts.tx_power_factor	= 0.50;						// TX power factor

	ts.pa_bias			= 0;//DEFAULT_PA_BIAS;			// Use lowest possible voltage as default
	ts.pa_cw_bias		= 0;//DEFAULT_PA_BIAS;			// Use lowest possible voltage as default (nonzero sets separate bias for CW mode)
	ts.freq_cal			= 0;							// Initial setting for frequency calibration
	ts.power_level		= PA_LEVEL_DEFAULT;			// See mchf_board.h for setting
	//
//	ts.codec_vol		= 0;						// Holder for codec volume
//	ts.codec_mute_state	= 0;						// Holder for codec mute state
//	ts.codec_was_muted = 0;							// Indicator that codec *was* muted
	//
	ts.powering_down	= 0;						// TRUE if powering down
	//
	ts.scope_speed		= 5;//SPECTRUM_SCOPE_SPEED_DEFAULT;	// default rate of spectrum scope update

	ts.waterfall_speed	= 15;//WATERFALL_SPEED_DEFAULT_SPI;		// default speed of update of the waterfall for parallel displays
	//
	ts.scope_filter		= 4;//SPECTRUM_SCOPE_FILTER_DEFAULT;	// default filter strength for spectrum scope
//!	ts.scope_trace_colour	= SPEC_COLOUR_TRACE_DEFAULT;	// default colour for the spectrum scope trace
//!	ts.scope_grid_colour	= SPEC_COLOUR_GRID_DEFAULT;		// default colour for the spectrum scope grid
//!	ts.scope_grid_colour_active = Grid;
//!	ts.scope_centre_grid_colour = SPEC_COLOUR_GRID_DEFAULT;		// color of center line of scope grid
//!	ts.scope_centre_grid_colour_active = Grid;
//!	ts.scope_scale_colour	= SPEC_COLOUR_SCALE_DEFAULT;	// default colour for the spectrum scope frequency scale at the bottom
	ts.scope_agc_rate	= 25;//SPECTRUM_SCOPE_AGC_DEFAULT;		// load default spectrum scope AGC rate
	ts.spectrum_db_scale = DB_DIV_10;					// default to 10dB/division
	//
	ts.menu_item		= 0;						// start out with a reasonable menu item
	//
	ts.radio_config_menu_enable = 0;				// TRUE if radio configuration menu is to be enabled
	//
	ts.cat_mode_active	= 0;						// TRUE if CAT mode is active
	//
	ts.xverter_mode		= 0;						// TRUE if transverter mode is active (e.g. offset of display)
	ts.xverter_offset	= 0;						// Frequency offset in transverter mode (added to frequency display)
	//
	ts.refresh_freq_disp	= 1;					// TRUE if frequency/color display is to be refreshed when next called - NORMALLY LEFT AT 0 (FALSE)!!!
													// This is NOT reset by the LCD function, but must be enabled/disabled externally
	//
	ts.pwr_80m_5w_adj	= 1;
	ts.pwr_60m_5w_adj	= 1;
	ts.pwr_40m_5w_adj	= 1;
	ts.pwr_30m_5w_adj	= 1;
	ts.pwr_20m_5w_adj	= 1;
	ts.pwr_17m_5w_adj	= 1;
	ts.pwr_15m_5w_adj	= 1;
	ts.pwr_12m_5w_adj	= 1;
	ts.pwr_10m_5w_adj	= 1;
	//
	ts.filter_cw_wide_disable		= 0;			// TRUE if wide filters are to be disabled in CW mode
	ts.filter_ssb_narrow_disable	= 0;			// TRUE if narrow (CW) filters are to be disabled in SSB mdoe
	ts.am_mode_disable				= 0;			// TRUE if AM mode is to be disabled
	//
	ts.tx_meter_mode	= 0;//METER_SWR;
	//
	ts.alc_decay		= 10;//ALC_DECAY_DEFAULT;		// ALC Decay (release) default value
	ts.alc_decay_var	= 10;//ALC_DECAY_DEFAULT;		// ALC Decay (release) default value
	ts.alc_tx_postfilt_gain		= 1;//ALC_POSTFILT_GAIN_DEFAULT;	// Post-filter added gain default (used for speech processor/ALC)
	ts.alc_tx_postfilt_gain_var		= 1;//ALC_POSTFILT_GAIN_DEFAULT;	// Post-filter added gain default (used for speech processor/ALC)
	ts.tx_comp_level	= 0;		// 0=Release Time/Pre-ALC gain manually adjusted, >=1:  Value calculated by this parameter
	//
	ts.freq_step_config		= 0;			// disabled both marker line under frequency and swapping of STEP buttons
	//
	ts.nb_disable		= 0;				// TRUE if noise blanker is to be disabled
	//
	ts.dsp_active		= 0;				// TRUE if DSP noise reduction is to be enabled
	ts.dsp_active_toggle	= 0xff;			// used to hold the button G2 "toggle" setting.
	ts.dsp_nr_delaybuf_len = 192;//DSP_NR_BUFLEN_DEFAULT;
	ts.dsp_nr_strength	= 0;				// "Strength" of DSP noise reduction (0 = weak)
	ts.dsp_nr_numtaps 	= 96;//DSP_NR_NUMTAPS_DEFAULT;		// default for number of FFT taps for noise reduction
	ts.dsp_notch_numtaps = 96;//DSP_NOTCH_NUMTAPS_DEFAULT;	// default for number of FFT taps for notch filter
	ts.dsp_notch_delaybuf_len =	104;//DSP_NOTCH_DELAYBUF_DEFAULT;
	ts.dsp_inhibit		= 1;				// TRUE if DSP is to be inhibited - power up with DSP disabled
	ts.dsp_inhibit_mute = 0;				// holder for "dsp_inhibit" during muting operations to allow restoration of previous state
	ts.dsp_timed_mute	= 0;				// TRUE if DSP is to be muted for a timed amount
	ts.dsp_inhibit_timing = 0;				// used to time inhibiting of DSP when it must be turned off for some reason
	ts.reset_dsp_nr		= 0;				// TRUE if DSP NR coefficients are to be reset when "audio_driver_set_rx_audio_filter()" is called
	ts.lcd_backlight_brightness = 0;		// = 0 full brightness
	ts.lcd_backlight_blanking = 0;			// MSB = 1 for auto-off of backlight, lower nybble holds time for auto-off in seconds
	//
	ts.tune_step		= 0;				// Used for press-and-hold step size changing mode
	ts.frequency_lock	= 0;				// TRUE if frequency knob is locked
	//
	ts.tx_disable		= 0;				// TRUE if transmitter is to be disabled
	ts.misc_flags1		= 0;				// Used to hold individual status flags, stored in EEPROM location "EEPROM_MISC_FLAGS1"
	ts.misc_flags2		= 0;				// Used to hold individual status flags, stored in EEPROM location "EEPROM_MISC_FLAGS2"
	ts.sysclock			= 0;				// This counts up from zero when the unit is powered up at precisely 100 Hz over the long term.  This
											// is NEVER reset and is used for timing certain events.
	ts.version_number_release	= 0;		// version release - used to detect firmware change
	ts.version_number_build = 0;			// version build - used to detect firmware change
	ts.nb_agc_time_const	= 0;			// used to calculate the AGC time constant
	ts.cw_offset_mode	= 0;				// CW offset mode (USB, LSB, etc.)
	ts.cw_lsb			= 0;				// Flag that indicates CW operates in LSB mode when TRUE
	ts.iq_freq_mode		= 0;				// used to set/configure the I/Q frequency/conversion mode
	ts.conv_sine_flag	= 0;				// FALSE until the sine tables for the frequency conversion have been built (normally zero, force 0 to rebuild)
	ts.lsb_usb_auto_select	= 0;			// holds setting of LSB/USB auto-select above/below 10 MHz
	ts.hold_off_spectrum_scope	= 0;		// this is a timer used to hold off updates of the spectrum scope when an SPI LCD display interface is used
	ts.lcd_blanking_time = 0;				// this holds the system time after which the LCD is blanked - if blanking is enabled
	ts.lcd_blanking_flag = 0;				// if TRUE, the LCD is blanked completely (e.g. backlight is off)
	ts.freq_cal_adjust_flag = 0;			// set TRUE if frequency calibration is in process
	ts.xvtr_adjust_flag = 0;				// set TRUE if transverter offset adjustment is in process
	ts.rx_muting = 0;						// set TRUE if audio output is to be muted
	ts.rx_blanking_time = 0;				// this is a timer used to delay the un-blanking of the audio after a large synthesizer tuning step
	ts.vfo_mem_mode = 0;					// this is used to record the VFO/memory mode (0 = VFO "A" = backwards compatibility)
											// LSB+6 (0x40) = 0:  VFO A,  1 = VFO B
											// LSB+7 (0x80) = 0:  Normal mode, 1 = SPLIT mode
											// Other bits are currently reserved
	ts.voltmeter_calibrate	= 100;//POWER_VOLTMETER_CALIBRATE_DEFAULT;	// Voltmeter calibration constant
	ts.thread_timer = 0;					// used to time thread
//!	ts.waterfall_color_scheme = WATERFALL_COLOR_DEFAULT;	// color scheme for waterfall display
	ts.waterfall_vert_step_size = 2;//WATERFALL_STEP_SIZE_DEFAULT;		// step size in waterfall display
//!	ts.waterfall_offset = WATERFALL_OFFSET_DEFAULT;		// Offset for waterfall display (brightness)
//!	ts.waterfall_contrast = WATERFALL_CONTRAST_DEFAULT;	// contrast setting for waterfall display
	ts.spectrum_scope_scheduler = 0;		// timer for scheduling the next update of the spectrum scope update
	ts.spectrum_scope_nosig_adjust = 20;	//SPECTRUM_SCOPE_NOSIG_ADJUST_DEFAULT;	// Adjustment for no signal adjustment conditions for spectrum scope
//!	ts.waterfall_nosig_adjust = WATERFALL_NOSIG_ADJUST_DEFAULT;		// Adjustment for no signal adjustment conditions for waterfall
	ts.waterfall_size	= 0;//WATERFALL_SIZE_DEFAULT;		// adjustment for waterfall size
	ts.fft_window_type = FFT_WINDOW_DEFAULT;			// FFT Windowing type
	ts.dvmode = 0;							// disable "DV" mode RX/TX functions by default
	ts.tx_audio_muting_timing = 0;			// timing value used for muting TX audio when keying PTT to suppress "click" or "thump"
	ts.tx_audio_muting_timer = 0;			// timer used for muting TX audio when keying PTT to suppress "click" or "thump"
	ts.tx_audio_muting_flag = 0;			// when TRUE, audio is to be muted after PTT/keyup
//!	ts.filter_disp_colour = FILTER_DISP_COLOUR_DEFAULT;	//
	ts.vfo_mem_flag = 0;					// when TRUE, memory mode is enabled
	ts.mem_disp = 0;						// when TRUE, memory display is enabled
}

QueueHandle_t 	hEspMessage;

//*----------------------------------------------------------------------------
//* Function Name       : audio_proc_hw_init
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_RESET
//*----------------------------------------------------------------------------
static void start_tasks(void)
{
	// Create TS Thread
	#if 1
    if(xTaskCreate((TaskFunction_t)touch_proc_task,"touch_proc", TS_TaskSTACK_SIZE, NULL, TS_TaskPRIORITY, &hTouchTask) != pdPASS)
    	printf("unable to create touch task\r\n");
	#endif

	#ifdef CONTEXT_DRIVER_UI
    if(xTaskCreate((TaskFunction_t)ui_proc_task,"gui_proc", GUI_TaskSTACK_SIZE, NULL, GUI_TaskPRIORITY, NULL) != pdPASS)
        printf("unable to create emwin_ui task\r\n");
    #endif

	#if 1
    if(xTaskCreate((TaskFunction_t)icc_proc_task,"icc_proc", GUI_TaskSTACK_SIZE, NULL, GUI_TaskPRIORITY, &hIccTask) != pdPASS)
       printf("unable to create icc task\r\n");
	#endif

	#ifdef ESP32_UART_TASK
    hEspMessage = xQueueCreate(5, sizeof(struct ESPMessage *));
    if(xTaskCreate((TaskFunction_t)esp32_proc_task,"esp32_proc", GUI_TaskSTACK_SIZE, NULL, GUI_TaskPRIORITY, NULL) != pdPASS)
    	printf("unable to create esp32_uart task\r\n");
	#endif

	#ifdef CONTEXT_AUDIO__
    if(xTaskCreate((TaskFunction_t)audio_proc_task,"audio_proc", GUI_TaskSTACK_SIZE, NULL, GUI_TaskPRIORITY, NULL) != pdPASS)
        printf("unable to create audio task\r\n");
    #endif
}

//*----------------------------------------------------------------------------
//* Function Name       : main
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_RESET
//*----------------------------------------------------------------------------
int main(void)
{
	/* Disable FMC Bank1 to avoid speculative/cache accesses */
	FMC_Bank1_R->BTCR[0] &= ~FMC_BCRx_MBKEN;

	/* Enable RCC PWR */
	__HAL_RCC_RTC_ENABLE();

	// ICC driver needs this
	__HAL_RCC_HSEM_CLK_ENABLE();

	/* Enable RTC back-up registers access */
	__HAL_RCC_RTC_CLK_ENABLE();
	HAL_PWR_EnableBkUpAccess();

    /* Configure the MPU attributes as Write Through */
    MPU_Config();

    /* Enable the CPU Cache */
    CPU_CACHE_Enable();

    HAL_Init();

    /* Configure the system clock to 400 MHz */
    SystemClock_Config();

    k_CalendarBkupInit();

    // Set radio public values
    TransceiverStateInit();

    /* Add Cortex-M7 user application code here */
    BSP_Initialized = BSP_Config();
    if(BSP_Initialized)
    {
    	// Init the SD Card hardware and its IRQ handler manager
    	Storage_Init();

    	// Define static tasks
    	start_tasks();

    	// Start scheduler
    	osKernelStart();
    }

    /* We should never get here as control is now taken by the scheduler */
    while (1)
    {
    	//BSP_LED_Toggle(LED_RED);
    	HAL_Delay(500);
    }
}
