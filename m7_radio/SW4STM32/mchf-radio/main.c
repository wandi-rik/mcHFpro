
#include "main.h"
#include "mchf_pro_board.h"
#include "version.h"
#include "radio_init.h"
#include "k_rtc.h"

#include "bsp.h"
#include "WM.h"

#include "ipc_proc.h"
#include "ui_proc.h"
#include "icc_proc.h"
#include "audio_proc.h"
#include "touch_proc.h"
#include "rotary_proc.h"

#if configAPPLICATION_ALLOCATED_HEAP == 1
__attribute__((section("heap_mem"))) uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
#endif

#define TS_TaskPRIORITY                 osPriorityHigh /* osPriorityNormal osPriorityRealtime */
#define TS_TaskSTACK_SIZE               (configMINIMAL_STACK_SIZE * 2)

#define GUI_TaskPRIORITY                osPriorityNormal /* osPriorityNormal osPriorityRealtime */
#define GUI_TaskSTACK_SIZE              (configMINIMAL_STACK_SIZE * 128)

uint8_t BSP_Initialized = 0;
uint32_t wakeup_pressed = 0;

// UI process
extern struct	UI_DRIVER_STATE			ui_s;

// DSP core state
struct TransceiverState 				ts;

TaskHandle_t 							hIccTask;
TaskHandle_t 							hTouchTask;

QueueHandle_t 							hEspMessage;

void NMI_Handler(void)
{
	Error_Handler(11);
}

void HardFault_Handler(void)
{
	printf( "====================\r\n");
	printf( "==== HARD FAULT ====\r\n");
	printf( "====================\r\n");

	NVIC_SystemReset();
}

void MemManage_Handler(void)
{
	Error_Handler(13);
}

void BusFault_Handler(void)
{
	Error_Handler(14);
}

void UsageFault_Handler(void)
{
	Error_Handler(15);
}

void DebugMon_Handler(void)
{
}

void SysTick_Handler(void)
{
	osSystickHandler();
}

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
  //if(BSP_Initialized)
  //{
    //printf( "%s(): BSP Error !!!\n", __func__ );
   // BSP_LED_On(LED_RED);
  //}
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
    if(xTaskCreate((TaskFunction_t)touch_proc_task, "touch_proc", TS_TaskSTACK_SIZE, NULL, TS_TaskPRIORITY, &hTouchTask) != pdPASS)
    	printf("unable to create touch task\r\n");
	#endif

	#ifdef CONTEXT_DRIVER_UI
    if(xTaskCreate((TaskFunction_t)ui_proc_task, "gui_proc", GUI_TaskSTACK_SIZE, NULL, GUI_TaskPRIORITY, NULL) != pdPASS)
        printf("unable to create emwin_ui task\r\n");
    #endif

	#if 1
    if(xTaskCreate((TaskFunction_t)icc_proc_task, "icc_proc", GUI_TaskSTACK_SIZE, NULL, GUI_TaskPRIORITY, &hIccTask) != pdPASS)
       printf("unable to create icc task\r\n");
	#endif

	#ifdef CONTEXT_ROTARY
    if(xTaskCreate((TaskFunction_t)rotary_proc, "enc_proc", TS_TaskSTACK_SIZE, NULL, TS_TaskPRIORITY, NULL) != pdPASS)
    printf("unable to create rotary task\r\n");
	#endif

	#ifdef CONTEXT_IPC_PROC
    hEspMessage = xQueueCreate(5, sizeof(struct ESPMessage *));
    if(xTaskCreate((TaskFunction_t)ipc_proc_task,"ipc_proc", GUI_TaskSTACK_SIZE, NULL, GUI_TaskPRIORITY, NULL) != pdPASS)
    	printf("unable to create ipc task\r\n");
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
	// Disable FMC Bank1 to avoid speculative/cache accesses
	FMC_Bank1_R->BTCR[0] &= ~FMC_BCRx_MBKEN;

	// Enable RCC PWR
	__HAL_RCC_RTC_ENABLE();

	// ICC driver needs this
	__HAL_RCC_HSEM_CLK_ENABLE();

	// Enable RTC back-up registers access
	__HAL_RCC_RTC_CLK_ENABLE();
	HAL_PWR_EnableBkUpAccess();

    // Configure the MPU attributes as Write Through
    MPU_Config();

    // Enable the CPU Cache
    CPU_CACHE_Enable();

    HAL_Init();

    // Configure the system clock to 400 MHz
    SystemClock_Config();

    // HW init
    if(BSP_Config() != 0)
    	goto stall_radio;

    k_CalendarBkupInit();

    // Set radio public values
    TransceiverStateInit();

    // Init the SD Card hardware and its IRQ handler manager
    Storage_Init();

    // Define static tasks
    start_tasks();

    // Do we need this at all ?
    BSP_Initialized = 1;

    // Start scheduler
    osKernelStart();

stall_radio:
    while(1);
}
