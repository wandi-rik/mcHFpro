/************************************************************************************
**                                                                                 **
**                             mcHF Pro QRP Transceiver                            **
**                         Krassi Atanassov - M0NKA 2012-2020                      **
**                            mail: djchrismarc@gmail.com                          **
**                                 twitter: @bph_co                                **
**---------------------------------------------------------------------------------**
**                                                                                 **
**  File name:                                                                     **
**  Description:                                                                   **
**  Last Modified:                                                                 **
**  Licence:                                                                       **
**          The mcHF project is released for radio amateurs experimentation,       **
**          non-commercial use only. All source files under GPL-3.0, unless        **
**          third party drivers specifies otherwise. Thank you!                    **
************************************************************************************/

#include "mchf_board.h"
#include "version.h"

#include "icc_proc.h"
#include "audio_proc.h"
#include "ui_driver.h"

#define HSEM_ID_0 (0U)

// Transceiver state public structure
__IO TransceiverState ts;

//*----------------------------------------------------------------------------
//* Function Name       : CriticalError
//* Object              : should never be here, really
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void CriticalError(unsigned long error)
{
	NVIC_SystemReset();
}

//*----------------------------------------------------------------------------
//* Function Name       : NMI_Handler
//* Object              :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void NMI_Handler(void)
{
	//printf("NMI_Handler called\n\r");
	CriticalError(1);
}

//*----------------------------------------------------------------------------
//* Function Name       : HardFault_Handler
//* Object              :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void HardFault_Handler(void)
{
	//printf("HardFault_Handler called\n\r");
	CriticalError(2);
}

//*----------------------------------------------------------------------------
//* Function Name       : MemManage_Handler
//* Object              :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void MemManage_Handler(void)
{
	//printf("MemManage_Handler called\n\r");
	CriticalError(3);
}

//*----------------------------------------------------------------------------
//* Function Name       : BusFault_Handler
//* Object              :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void BusFault_Handler(void)
{
	//printf("BusFault_Handler called\n\r");
	CriticalError(4);
}

//*----------------------------------------------------------------------------
//* Function Name       : UsageFault_Handler
//* Object              :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void UsageFault_Handler(void)
{
	//printf("UsageFault_Handler called\n\r");
	CriticalError(5);
}

//*----------------------------------------------------------------------------
//* Function Name       : SVC_Handler
//* Object              :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void SVC_Handler(void)
{
	//printf("SVC_Handler called\n\r");
	CriticalError(6);
}

//*----------------------------------------------------------------------------
//* Function Name       : DebugMon_Handler
//* Object              :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void DebugMon_Handler(void)
{
	//printf("DebugMon_Handler called\n\r");
	CriticalError(7);
}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void SysTick_Handler(void)
{
	HAL_IncTick();
}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void HSEM2_IRQHandler(void)
{
	HAL_HSEM_IRQHandler();
}

//*----------------------------------------------------------------------------
//* Function Name       : main
//* Object              :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
int main(void)
{
	// Configure the Cortex-M4 ART Base address to D2_AXISRAM_BASE : 0x10000000
    __HAL_ART_CONFIG_BASE_ADDRESS(D2_AXISRAM_BASE);

    // HW semaphore Clock enable
    __HAL_RCC_HSEM_CLK_ENABLE();

    // Activate HSEM notification for Cortex-M4
    HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));

	// Domain D2 goes to STOP mode (Cortex-M4 in deep-sleep) waiting for Cortex-M7 to
	// perform system initialisation (system clock config, external memory configuration.. )
    HAL_PWREx_ClearPendingEvent();
    HAL_PWREx_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFE, PWR_D2_DOMAIN);

	HAL_Init();

	// Init debug print in shared mode
	printf_init(1);

	// Clear Flags generated during the wakeup notification
	HSEM_COMMON->ICR |= ((uint32_t)__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));
	HAL_NVIC_ClearPendingIRQ(HSEM2_IRQn);

	// Init allocated leds
	BSP_LED_Init(LED_RED);
	BSP_LED_Init(LED_BLUE);

	// Core power on
	BSP_LED_On(LED_RED);

	printf("--------------------------------------\r\n");
    printf("--- Debug Print Session on (dsp) ---\r\n");
	printf("Firmware v: %d.%d.%d.%d\r\n", MCHF_D_VER_MAJOR, MCHF_D_VER_MINOR, MCHF_D_VER_RELEASE, MCHF_D_VER_BUILD);

	// ICC driver init
	icc_proc_hw_init();

main_loop:
	icc_proc_task(NULL);
	ui_driver_thread();
	goto main_loop;
}
