/************************************************************************************
**                                                                                 **
**                             mcHF Pro QRP Transceiver                            **
**                         Krassi Atanassov - M0NKA 2012-2019                      **
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

#include "mchf_pro_board.h"
#include "main.h"

#define PWR_CFG_SMPS    0xCAFECAFE
#define PWR_CFG_LDO     0x5ACAFE5A

typedef struct pwr_db
{
  __IO uint32_t t[0x30/4];
  __IO uint32_t PDR1;
}PWDDBG_TypeDef;

/* Private macro -------------------------------------------------------------*/
#define PWDDBG                          ((PWDDBG_TypeDef*)PWR)
#define DEVICE_IS_CUT_2_1()             (HAL_GetREVID() & 0x21ff) ? 1 : 0

// Core unique regs loaded to RAM
struct	CM7_CORE_DETAILS		ccd;

// Public radio state
struct	TRANSCEIVER_STATE_UI	tsu;

__IO  uint32_t SystemClock_MHz 		= 480;
__IO  uint32_t SystemClock_changed 	= 0;

/**
  * @brief  System Clock Configuration to 400MHz
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 400000000 (CM7 CPU Clock)
  *            HCLK(Hz)                       = 200000000 (CM4 CPU, AXI and AHBs Clock)
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
  * @param  None
  * @retval None
  */
static void SystemClock_Config_400MHz(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
	HAL_StatusTypeDef ret = HAL_OK;

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
		Error_Handler(181);
	}
}

/**
  * @brief  System Clock Configuration to 480MHz
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 480000000 (CM7 CPU Clock)
  *            HCLK(Hz)                       = 240000000 (CM4 CPU, AXI and AHBs Clock)
  *            AHB Prescaler                  = 2
  *            D1 APB3 Prescaler              = 2 (APB3 Clock  120MHz)
  *            D2 APB1 Prescaler              = 2 (APB1 Clock  120MHz)
  *            D2 APB2 Prescaler              = 2 (APB2 Clock  120MHz)
  *            D3 APB4 Prescaler              = 2 (APB4 Clock  120MHz)
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 5
  *            PLL_N                          = 192
  *            PLL_P                          = 2
  *            PLL_Q                          = 4
  *            PLL_R                          = 2
  * @param  None
  * @retval None
  */
static void SystemClock_Config_480MHz(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
	HAL_StatusTypeDef ret = HAL_OK;

	/* Enable HSE Oscillator and activate PLL with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
	RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

	RCC_OscInitStruct.PLL.PLLM = 5;
	RCC_OscInitStruct.PLL.PLLN = 192;
	RCC_OscInitStruct.PLL.PLLFRACN = 0;
	RCC_OscInitStruct.PLL.PLLP = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	RCC_OscInitStruct.PLL.PLLQ = 4;

	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
	ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
	if(ret != HAL_OK)
	{
		Error_Handler(180);
	}
}

void SystemClockChange_Handler(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	if(SystemClock_changed != 0)
	{
		/* Select HSE  as system clock source to allow modification of the PLL configuration */
		RCC_ClkInitStruct.ClockType       = RCC_CLOCKTYPE_SYSCLK;
		RCC_ClkInitStruct.SYSCLKSource    = RCC_SYSCLKSOURCE_HSE;
		if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
		{
			/* Initialization Error */
			Error_Handler(178);
		}

		if(SystemClock_MHz == 400)
		{
			__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);
			while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

			SystemClock_Config_480MHz();
			SystemClock_MHz = 480;
		}
		else
		{
			SystemClock_Config_400MHz();

			__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
			while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

			SystemClock_MHz = 400;
		}

		/* PLL1  as system clock source to allow modification of the PLL configuration */
		RCC_ClkInitStruct.ClockType       = RCC_CLOCKTYPE_SYSCLK;
		RCC_ClkInitStruct.SYSCLKSource    = RCC_SYSCLKSOURCE_PLLCLK;
		if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
		{
			/* Initialization Error */
			Error_Handler(179);
		}

		SystemClock_changed = 0;
	}
}

void SystemClock_Config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	HAL_StatusTypeDef ret = HAL_OK;

	//#if (USE_VOS0_480MHZ_OVERCLOCK == 1)
	//#if (BOARD_HW_CONFIG_IS_LDO == 1) && (BOARD_HW_CONFIG_IS_DIRECT_SMPS == 0)
	#if defined(USE_PWR_LDO_SUPPLY)
	/*!< Supply configuration update enable */
	HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
	//#else
	//  #error "Please make sure that the STM32H747I-DISCO Board has been modified to match the LDO configuration then set the define BOARD_HW_CONFIG_IS_LDO to 1 and BOARD_HW_CONFIG_IS_DIRECT_SMPS set to 0 to confirm the HW config"
	//#endif  /* (BOARD_HW_CONFIG_IS_LDO == 1) && (BOARD_HW_CONFIG_IS_DIRECT_SMPS == 0) */
	#else
	//#if (BOARD_HW_CONFIG_IS_LDO == 0) && (BOARD_HW_CONFIG_IS_DIRECT_SMPS == 1)
	/*!< Supply configuration update enable */
	HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);
	//#else
	//  #error "Please make sure that the STM32H747I-DISCO Board has been modified to match the Direct SMPS configuration then set the define BOARD_HW_CONFIG_IS_LDO to 0 and BOARD_HW_CONFIG_IS_DIRECT_SMPS set to 1 to confirm the HW config"
	#endif  /* (BOARD_HW_CONFIG_IS_LDO == 0) && (BOARD_HW_CONFIG_IS_DIRECT_SMPS == 1) */
	//#endif /* USE_VOS0_480MHZ_OVERCLOCK */

	/* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

	//#if (USE_VOS0_480MHZ_OVERCLOCK == 1)
	if(SystemClock_MHz == 480)
	{
		__HAL_RCC_SYSCFG_CLK_ENABLE();
		__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);
		while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

		SystemClock_Config_480MHz();
	}
	else
	{
		//#else
		SystemClock_Config_400MHz();
		//#endif /* (USE_VOS0_480MHZ_OVERCLOCK == 1) */
	}

	/* Select PLL as system clock source and configure  bus clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_PCLK1 | \
								   RCC_CLOCKTYPE_PCLK2  | RCC_CLOCKTYPE_D3PCLK1);

	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
	RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
	ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
	if(ret != HAL_OK)
	{
		Error_Handler(177);
	}

	/*
  	  Note : The activation of the I/O Compensation Cell is recommended with communication  interfaces
          (GPIO, SPI, FMC, QSPI ...)  when  operating at  high frequencies(please refer to product datasheet)
          The I/O Compensation Cell activation  procedure requires :
        - The activation of the CSI clock
        - The activation of the SYSCFG clock
        - Enabling the I/O Compensation Cell : setting bit[0] of register SYSCFG_CCCSR
	 */

	/*activate CSI clock mondatory for I/O Compensation Cell*/
	__HAL_RCC_CSI_ENABLE() ;

	/* Enable SYSCFG clock mondatory for I/O Compensation Cell */
	__HAL_RCC_SYSCFG_CLK_ENABLE() ;

	/* Enables the I/O Compensation Cell */
	HAL_EnableCompensationCell();
}

#if 0
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
#endif

void MPU_Config(void)
{
	MPU_Region_InitTypeDef MPU_InitStruct;

	// Disable the MPU
	HAL_MPU_Disable();

	// Setup Flash - launcher and radio code execution
	MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress      = FLASH_BASE;					// 0x08000000
	MPU_InitStruct.Size             = MPU_REGION_SIZE_2MB;			// 2MB
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable     = MPU_ACCESS_BUFFERABLE;
	MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
	MPU_InitStruct.Number           = MPU_REGION_NUMBER0;
	MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
	MPU_InitStruct.SubRegionDisable = 0x00;
	MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	// Setup SDRAM - emWin video buffers
	MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress      = SDRAM_DEVICE_ADDR;			// 0xD0000000
	MPU_InitStruct.Size             = MPU_REGION_SIZE_32MB;			// 32MB
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
	MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
	MPU_InitStruct.Number           = MPU_REGION_NUMBER1;
	MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
	MPU_InitStruct.SubRegionDisable = 0x00;
	MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	// Setup D3 SRAM - OpenAMP core to core comms
	MPU_InitStruct.Enable 			= MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress 		= D3_SRAM_BASE;					// 0x38000000
	MPU_InitStruct.Size 			= MPU_REGION_SIZE_64KB;			// 64KB
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable 	= MPU_ACCESS_NOT_BUFFERABLE;
	MPU_InitStruct.IsCacheable 		= MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.IsShareable 		= MPU_ACCESS_SHAREABLE;
	MPU_InitStruct.Number 			= MPU_REGION_NUMBER2;
	MPU_InitStruct.TypeExtField 	= MPU_TEX_LEVEL0;
	MPU_InitStruct.SubRegionDisable = 0x00;
	MPU_InitStruct.DisableExec 		= MPU_INSTRUCTION_ACCESS_DISABLE;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	// Setup AXI SRAM - OS heap
	MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress      = D1_AXISRAM_BASE;				// 0x24000000
	MPU_InitStruct.Size             = MPU_REGION_SIZE_512KB;		// 512KB
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
	MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
	MPU_InitStruct.Number           = MPU_REGION_NUMBER3;
	MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
	MPU_InitStruct.SubRegionDisable = 0x00;
	MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	#if 0
	// Setup ITCM RAM - OS code, interrupt handlers
	MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress      = D1_ITCMRAM_BASE;				// 0x00000000
	MPU_InitStruct.Size             = MPU_REGION_SIZE_64KB;			// 64KB
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
	MPU_InitStruct.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;
	MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
	MPU_InitStruct.Number           = MPU_REGION_NUMBER4;
	MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
	MPU_InitStruct.SubRegionDisable = 0x00;
	MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	// Setup DTCM RAM - DMA buffers
	MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress      = D1_DTCMRAM_BASE;				// 0x20000000
	MPU_InitStruct.Size             = MPU_REGION_SIZE_128KB;		// 128KB
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
	MPU_InitStruct.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;
	MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
	MPU_InitStruct.Number           = MPU_REGION_NUMBER5;
	MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
	MPU_InitStruct.SubRegionDisable = 0x00;
	MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);
	#endif

    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

#if 0

#include "stm32h7xx_hal_rcc.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_cortex.h"
#include "stm32h7xx_hal_pwr.h"
#include "stm32h7xx_hal_flash_ex.h"

// Sensitive drivers HW init here ;(
#include "touch_driver.h"
#include "api_driver.h"
#include "rotary_driver.h"
#include "keypad_driver.h"
#include "net_driver.h"

extern void ItmInitAsm(void);

//*----------------------------------------------------------------------------
//* Function Name       : mchf_pro_board_debug_led_init
//* Object              : use HW LED to check H7 problems
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: anyone!
//*----------------------------------------------------------------------------
void mchf_pro_board_debug_led_init(void)
{
#ifdef STM32H753xx
	GPIO_InitTypeDef  	GPIO_InitStruct;

	__HAL_RCC_GPIOD_CLK_ENABLE();

	GPIO_InitStruct.Pin 		= GPIO_PIN_7;
	GPIO_InitStruct.Mode 		= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull 		= GPIO_NOPULL;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	// LED off
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
#endif
}

//*----------------------------------------------------------------------------
//* Function Name       : mchf_pro_board_blink_if_alive
//* Object              : debug hw problems by blinking the LCD backlight
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: anyone!
//*----------------------------------------------------------------------------
void mchf_pro_board_blink_if_alive(uchar flags)
{
#ifdef STM32H753xx
	GPIO_InitTypeDef  	GPIO_InitStruct;
	ulong 				i;

	// Need init ?
	if((flags & 0x01) == 0x01)
	{
		__HAL_RCC_GPIOF_CLK_ENABLE();

		// LCD Backlight PF9
		GPIO_InitStruct.Pin 		= GPIO_PIN_9;
		GPIO_InitStruct.Mode 		= GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull 		= GPIO_NOPULL;
		GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_RESET);
	}

	// Need stalling and blink ?
	if((flags & 0x02) == 0x02)
	{
		while(1)
		{
			HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_9);
			//printf("---\r\n");

			for(i = 0; i < 0x5FFFFFF; i++)
				__asm(".hword 0x46C0");
		}
	}

	// Just light on
	if((flags & 0x04) == 0x04)
	{
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_SET);
	}
#endif
}

//*----------------------------------------------------------------------------
//* Function Name       : mchf_pro_board_read_cpu_details
//* Object              : Read specific CPU regs before MMU remap
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void mchf_pro_board_read_cpu_details(void)
{

	ulong	*p_uniq = (ulong  *)CHIP_UNIQUE_ID;
	ushort 	*p_fls 	= (ushort *)CHIP_FLS_SIZE;

	ccd.wafer_coord 	= *(p_uniq + 0x00);
	ccd.wafer_number	= *(p_uniq + 0x04);
	ccd.lot_number		= *(p_uniq + 0x08);
	ccd.fls_size 		= *(p_fls  + 0x00);
}

//*----------------------------------------------------------------------------
//* Function Name       : mchf_pro_board_start_gpio_clocks
//* Object              : GPIO clocks on
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void mchf_pro_board_start_gpio_clocks(void)
{
#ifdef STM32H753xx
	// Enable GPIOs clocks
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOI_CLK_ENABLE();
#endif

#ifdef BOARD_DISCO
	//Extra ports on the BGA chip, so enable clocks
	__HAL_RCC_GPIOJ_CLK_ENABLE();
	__HAL_RCC_GPIOK_CLK_ENABLE();
#endif
}

//*----------------------------------------------------------------------------
//* Function Name       : mchf_pro_board_mpu_config
//* Object              : MPU setup
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void mchf_pro_board_mpu_config(void)
{
#ifdef STM32H753xx
  MPU_Region_InitTypeDef MPU_InitStruct;

  // Disable the MPU
  HAL_MPU_Disable();

  // Layer buffers - 0xC0000000, SDRAM, Bank1
  MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress      = 0xC0000000;
  MPU_InitStruct.Size             = MPU_REGION_SIZE_4MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number           = MPU_REGION_NUMBER0;					// table entry
  MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

   // Stack and data vars - 0x24000000 - 0x2407FFFF 	512k	AXI SRAM
   MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
   MPU_InitStruct.BaseAddress      = 0x24000000;
   MPU_InitStruct.Size             = MPU_REGION_SIZE_512KB;
   MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
   MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
   MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
   MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
   MPU_InitStruct.Number           = MPU_REGION_NUMBER1;				// table entry
   MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
   MPU_InitStruct.SubRegionDisable = 0x00;
   MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
   HAL_MPU_ConfigRegion(&MPU_InitStruct);

   // 0x30000000 - 0x3001FFFF 	128k	SRAM1
   MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
   MPU_InitStruct.BaseAddress      = 0x30000000;
   MPU_InitStruct.Size             = MPU_REGION_SIZE_128KB;
   MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
   MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
   MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
   MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
   MPU_InitStruct.Number           = MPU_REGION_NUMBER2;				// table entry
   MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
   MPU_InitStruct.SubRegionDisable = 0x00;
   MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
   HAL_MPU_ConfigRegion(&MPU_InitStruct);

   // 0x30020000 - 0x3003FFFF 	128k	SRAM2
   MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
   MPU_InitStruct.BaseAddress      = 0x30020000;
   MPU_InitStruct.Size             = MPU_REGION_SIZE_128KB;
   MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
   MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
   MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
   MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
   MPU_InitStruct.Number           = MPU_REGION_NUMBER3;				// table entry
   MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
   MPU_InitStruct.SubRegionDisable = 0x00;
   MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
   HAL_MPU_ConfigRegion(&MPU_InitStruct);

   // 0x30040000 - 0x30047FFF 	32k	SRAM3
   MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
   MPU_InitStruct.BaseAddress      = 0x30040000;
   MPU_InitStruct.Size             = MPU_REGION_SIZE_32KB;
   MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
   MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
   MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
   MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
   MPU_InitStruct.Number           = MPU_REGION_NUMBER4;				// table entry
   MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
   MPU_InitStruct.SubRegionDisable = 0x00;
   MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
   HAL_MPU_ConfigRegion(&MPU_InitStruct);

   // 0x38000000 - 0x3800FFFF 	64k		SRAM4
   MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
   MPU_InitStruct.BaseAddress      = 0x38000000;
   MPU_InitStruct.Size             = MPU_REGION_SIZE_64KB;
   MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
   MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
   MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
   MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
   MPU_InitStruct.Number           = MPU_REGION_NUMBER5;				// table entry
   MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
   MPU_InitStruct.SubRegionDisable = 0x00;
   MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
   HAL_MPU_ConfigRegion(&MPU_InitStruct);

   // Enable the MPU
   HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
#endif
}

//*----------------------------------------------------------------------------
//* Function Name       : mchf_pro_board_cpu_cache_enable
//* Object              : CPU L1-Cache enable
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void mchf_pro_board_cpu_cache_enable(void)
{
#ifdef STM32H753xx
	// Enable I-Cache
	SCB_EnableICache();

	#ifndef H7_DISABLE_DATA_CACHE
  	  // Enable D-Cache
  	  SCB_EnableDCache();
	#endif
#endif
}

//*----------------------------------------------------------------------------
//* Function Name       : mchf_pro_board_system_clock_config
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
uchar mchf_pro_board_system_clock_config(uchar clk_src)
{
#ifdef STM32H753xx
	RCC_ClkInitTypeDef			RCC_ClkInitStruct;
	RCC_OscInitTypeDef 			RCC_OscInitStruct;
	RCC_PeriphCLKInitTypeDef 	PeriphClkInitStruct;
	//
	HAL_StatusTypeDef 			ret = HAL_OK;

	// Supply configuration update enable
	MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);

	//Configure the main internal regulator output voltage
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	while ((PWR->D3CR & (PWR_D3CR_VOSRDY)) != PWR_D3CR_VOSRDY)
	{
		// ToDo: any timeout or handling ?
	}

	switch(clk_src)
	{
		// Internal
		case 0:
			__HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSI);
			RCC_OscInitStruct.HSIState 			= RCC_HSI_ON;
			RCC_OscInitStruct.OscillatorType	= RCC_OSCILLATORTYPE_HSI;
			RCC_OscInitStruct.PLL.PLLSource 	= RCC_PLLSOURCE_HSI;
			break;

		// XTAL
		case 1:
			__HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
			RCC_OscInitStruct.HSEState 			= RCC_HSE_ON;
			RCC_OscInitStruct.OscillatorType	= RCC_OSCILLATORTYPE_HSE;
			RCC_OscInitStruct.PLL.PLLSource 	= RCC_PLLSOURCE_HSE;
			break;

		// TCXO
		case 2:
			__HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
			RCC_OscInitStruct.HSEState 			= RCC_HSE_BYPASS;
			RCC_OscInitStruct.OscillatorType	= RCC_OSCILLATORTYPE_HSE;
			RCC_OscInitStruct.PLL.PLLSource 	= RCC_PLLSOURCE_HSE;
			break;

		default:
			return 20;
	}

	RCC_OscInitStruct.PLL.PLLState 		= RCC_PLL_ON;
	//
	RCC_OscInitStruct.PLL.PLLM 			= 1;
	RCC_OscInitStruct.PLL.PLLN 			= 50;
	RCC_OscInitStruct.PLL.PLLP 			= 2;
	RCC_OscInitStruct.PLL.PLLR 			= 2;
	RCC_OscInitStruct.PLL.PLLQ 			= 2;
	//
	RCC_OscInitStruct.PLL.PLLVCOSEL 	= RCC_PLL1VCOWIDE;
	RCC_OscInitStruct.PLL.PLLRGE 		= RCC_PLL1VCIRANGE_3;	//RCC_PLL1VCIRANGE_2;
	ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
	if(ret != HAL_OK)
		return 1;

	// Select PLL as system clock source and configure  bus clocks dividers
	RCC_ClkInitStruct.ClockType = 	(RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_PCLK1 | \
									 RCC_CLOCKTYPE_PCLK2  | RCC_CLOCKTYPE_D3PCLK1);

	#ifdef H7_200MHZ
	RCC_ClkInitStruct.SYSCLKDivider 	= RCC_SYSCLK_DIV2;
	#endif
	#ifdef H7_400MHZ
	RCC_ClkInitStruct.SYSCLKDivider 	= RCC_SYSCLK_DIV1;
	#endif

	RCC_ClkInitStruct.SYSCLKSource 		= RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider 	= RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB3CLKDivider 	= RCC_APB3_DIV2;
	RCC_ClkInitStruct.APB1CLKDivider 	= RCC_APB1_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider 	= RCC_APB2_DIV2;
	RCC_ClkInitStruct.APB4CLKDivider 	= RCC_APB4_DIV2;

	ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
	if(ret != HAL_OK)
		return 2;

	//
	// Peripheral clocks - SPI2, use very slow clock for initial testing
	//
	PeriphClkInitStruct.PeriphClockSelection 	= RCC_PERIPHCLK_SPI2;
	PeriphClkInitStruct.PLL2.PLL2M 				= 1;
	PeriphClkInitStruct.PLL2.PLL2N 				= 10;
	PeriphClkInitStruct.PLL2.PLL2P 				= 16;
	PeriphClkInitStruct.PLL2.PLL2Q 				= 2;
	PeriphClkInitStruct.PLL2.PLL2R 				= 2;
	PeriphClkInitStruct.PLL2.PLL2RGE 			= RCC_PLL2VCIRANGE_3;
	PeriphClkInitStruct.PLL2.PLL2VCOSEL 		= RCC_PLL2VCOMEDIUM;
	PeriphClkInitStruct.PLL2.PLL2FRACN 			= 0;
	PeriphClkInitStruct.PLL3.PLL3M 				= 32;
	PeriphClkInitStruct.PLL3.PLL3N 				= 140;
	PeriphClkInitStruct.PLL3.PLL3P 				= 2;
	PeriphClkInitStruct.PLL3.PLL3Q 				= 2;
	PeriphClkInitStruct.PLL3.PLL3R 				= 2;
	PeriphClkInitStruct.PLL3.PLL3RGE 			= RCC_PLL3VCIRANGE_3;
	PeriphClkInitStruct.PLL3.PLL3VCOSEL 		= RCC_PLL3VCOWIDE;
	PeriphClkInitStruct.PLL3.PLL3FRACN 			= 0;
	PeriphClkInitStruct.Spi123ClockSelection 	= RCC_SPI123CLKSOURCE_PLL2;

	ret = HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
	if(ret != HAL_OK)
		return 3;
#endif
	return 0;
}

//*----------------------------------------------------------------------------
//* Function Name       : mchf_pro_board_rtc_clock_config
//* Object              : start rtc osc and pll separate from the main clock
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
uchar mchf_pro_board_rtc_clock_config(uchar clk_src)
{
#ifdef STM32H753xx
	RCC_OscInitTypeDef        RCC_OscInitStruct;
	RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

	/*##-1- Enables access to the backup domain ######*/
	/* To enable access on RTC registers */
	HAL_PWR_EnableBkUpAccess();
	/*##-2- Configure LSE/LSI as RTC clock source ###############################*/
	if(clk_src == EXT_32KHZ_XTAL)
	{
		RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
		RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
		RCC_OscInitStruct.LSEState = RCC_LSE_ON;
		RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
		if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
			return 1;

		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
		PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
		if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
			return 2;
	}
	else
	{
		RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
		RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
		RCC_OscInitStruct.LSIState = RCC_LSI_ON;
		RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
		if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
			return 1;

		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
		PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
		if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
			return 2;
	}
	//##-3- Enable RTC peripheral Clocks #######################################
	// Enable RTC Clock
	__HAL_RCC_RTC_ENABLE();

    // Enable write access to Backup domain
    PWR->CR1 |= PWR_CR1_DBP;
    while((PWR->CR1 & PWR_CR1_DBP) == RESET)
    {
    }

    // Enable BKPRAM clock
    __HAL_RCC_BKPRAM_CLK_ENABLE();

    // Enable the Backup SRAM low power Regulator
    HAL_PWREx_EnableBkUpReg();

    // Enable virtual eeprom
    tsu.eeprom_init_done = 1;
#endif

	return 0;
}

//*----------------------------------------------------------------------------
//* Function Name       : mchf_pro_board_rtc_clock_disable
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void mchf_pro_board_rtc_clock_disable(void)
{

}

//*----------------------------------------------------------------------------
//* Function Name       : mchf_pro_board_swo_init
//* Object              : Debug print via ITM(SWO pin)
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void mchf_pro_board_swo_init(void)
{
	ItmInitAsm();

	printf("--- Debug Print Session on ---\r\n");
	printf("reason for reset: 0x%02x (0x9C - watchdog, 0x98 - software)\r\n",tsu.reset_reason);
}

//*----------------------------------------------------------------------------
//* Function Name       : mchf_pro_board_mco2_on
//* Object              : Output 24.576 Mhz on PC9 for the Quadrature upconverter
//* Input Parameters    : use PLLSAI clock instead of PLLI2S - works somehow ?
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void mchf_pro_board_mco2_on(void)
{

}

//*----------------------------------------------------------------------------
//* Function Name       : mchf_pro_board_sensitive_hw_init
//* Object              :
//* Notes    			: On H7 hardware will crash if init is done in a task
//* Notes   			: during runtime, so init here
//* Notes    			:
//* Context    			: CONTEXT_RESET_VECTOR
//*----------------------------------------------------------------------------
void mchf_pro_board_sensitive_hw_init(void)
{
	// API driver SPI and GPIOs
	api_driver_hw_init();
	//
	// Digitizer driver I2C
	ft5x06_i2c_init();
	//
	// Rotary driver timers
	rotary_driver_hw_init();
	//
	// Keypad driver GPIO
	#ifdef CONTEXT_DRIVER_KEYPAD
	keypad_driver_init();
	#endif
	//
	// Net driver
	#ifdef CONTEXT_NET
	net_driver_hw_init();
	#endif
}

//*----------------------------------------------------------------------------
//* Function Name       : WRITE_EEPROM
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: anyone!
//*----------------------------------------------------------------------------
void WRITE_EEPROM(ushort addr,uchar value)
{
	uchar *bkp = (uchar *)EEP_BASE;

	if(!tsu.eeprom_init_done)
		return;

	if(addr > 0xFFF)
		return;

	// Write to BackUp SRAM
	*(bkp + addr) = value;
}

//*----------------------------------------------------------------------------
//* Function Name       : READ_EEPROM
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: anyone!
//*----------------------------------------------------------------------------
uchar READ_EEPROM(ushort addr)
{
	uchar ret;
	uchar *bkp = (uchar *)EEP_BASE;

	if(!tsu.eeprom_init_done)
		return 0xFF;

	if(addr > 0xFFF)
		return 0xFF;

	// Read BackUp SRAM
	return *(bkp + addr);
}
#endif
