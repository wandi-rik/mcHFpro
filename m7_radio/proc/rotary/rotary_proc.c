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

#include "mchf_pro_board.h"

#ifdef CONTEXT_ROTARY

#include "rotary_proc.h"

//#include "api_driver.h"
#include "desktop\smeter\ui_controls_smeter.h"

#include "gui.h"
#include "dialog.h"

#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_rcc.h"
#include "stm32h7xx_hal_rcc_ex.h"
#include "stm32h7xx_hal_tim.h"
#include "stm32h7xx_hal_tim_ex.h"
//
TIM_HandleTypeDef htim1;
//TIM_HandleTypeDef htim4;

#define TIM_PERIOD	0xF

ushort audio_old = 0;
ushort freq_old = 0;

uchar front_init_done = 0;

// Public radio state
extern struct	TRANSCEIVER_STATE_UI	tsu;
extern struct	UI_DRIVER_STATE			ui_s;
extern struct 	S_METER					sm;

// ---------------------------------
// Block s-meter refresh exports
//extern uchar	rotary_block;
//extern ushort	rotary_timer;
// ---------------------------------

#ifdef USE_SIDE_ENC_FOR_S_METER
extern ulong s_met_pos;
#endif

static void rotary_init_side_encoder_switch_pin(void)
{
#if 0
	GPIO_InitTypeDef  	GPIO_InitStruct;

	GPIO_InitStruct.Pin 		= GPIO_PIN_13;
	GPIO_InitStruct.Mode 		= GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull 		= GPIO_PULLUP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif
}

static void rotary_check_side_encoder_switch(void)
{
#if 0
	// Encoder button clicked ?
	if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13))
	{
		while(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13));

		//printf("side encoder clicked\r\n");

		// Toggle UI driver state(only enter from desktop)
		if(ui_s.req_state == MODE_DESKTOP)
		{
			ui_s.req_state = MODE_SIDE_ENC_MENU;
			return;
		}

		// Do not return from Menu mode
		if(ui_s.req_state == MODE_SIDE_ENC_MENU)
			ui_s.req_state = MODE_DESKTOP;
	}
#endif
}

static void rotary_update_audio_publics(int pot_diff)
{
#if 0
	// Update public volume
	if(pot_diff < 0)
	{
		if(tsu.band[tsu.curr_band].volume > 0)
			tsu.band[tsu.curr_band].volume += pot_diff;
	}
	else
	{
		if(tsu.band[tsu.curr_band].volume < 17)
			tsu.band[tsu.curr_band].volume += pot_diff;
	}
	// Save band info to eeprom
	save_band_info();
	//
	// Set request flag - now in volume control, so 'Mute' works better!
	//tsu.update_audio_dsp_req = 1;
	//
#endif
}

static void rotary_update_side_enc_menu_publics(int pot_diff)
{
#if 0
	if(pot_diff > 0)
	{
		GUI_StoreKeyMsg(GUI_KEY_RIGHT,1);
		GUI_StoreKeyMsg(GUI_KEY_RIGHT,0);
	}
	else
	{
		GUI_StoreKeyMsg(GUI_KEY_LEFT,1);
		GUI_StoreKeyMsg(GUI_KEY_LEFT,0);
	}
#endif
}

static void rotary_check_side_enc(void)
{
#if 0
	ushort 	cnt;
	int		pot_diff = 0;

	cnt = __HAL_TIM_GET_COUNTER(&htim3);
	if(audio_old == cnt)
		return;

	//printf("---------------------------------\r\n");
	//printf("cnt = %d\r\n",cnt);

	if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim3))
		pot_diff = -1;
	else
		pot_diff = +1;

	//printf("pot_diff = %d\r\n",pot_diff);

	// Do not update publics while in Menu(or other non desktop modes, where UI is not painted and processed)
	if(ui_s.cur_state == MODE_DESKTOP)
		rotary_update_audio_publics(pot_diff);

	if(ui_s.cur_state == MODE_SIDE_ENC_MENU)
		rotary_update_side_enc_menu_publics(pot_diff);

	// Flag preventing calling too often
	audio_old = cnt;
#endif
}

static void rotary_update_freq_publics(int pot_diff)
{
	// Block s-meter refresh
	sm.rotary_timer = 0;
	sm.rotary_block	= 1;

	// Update public volume
	if(pot_diff < 0)
	{
		if(tsu.band[tsu.curr_band].fixed_mode)
		{
			// ----------------------------------------
			// Limiter based on decoder mode
			if(tsu.band[tsu.curr_band].demod_mode == DEMOD_LSB)
			{
				if(tsu.band[tsu.curr_band].nco_freq < 24000)
					tsu.band[tsu.curr_band].nco_freq += tsu.band[tsu.curr_band].step;
			}
			if(tsu.band[tsu.curr_band].demod_mode == DEMOD_USB)
			{
				if(tsu.band[tsu.curr_band].nco_freq < 20000)
					tsu.band[tsu.curr_band].nco_freq += tsu.band[tsu.curr_band].step;
			}
			// Other modes handling...

			// Set request
			tsu.update_nco_dsp_req = 1;

			//printf("nco freq: %d\r\n",tsu.nco_freq);
		}
		else
		{
			if(tsu.band[tsu.curr_band].active_vfo == 0)
				//tsu.vfo_a += tsu.step;
				tsu.band[tsu.curr_band].vfo_a += tsu.band[tsu.curr_band].step;
			else
				//tsu.vfo_b += tsu.step;
				tsu.band[tsu.curr_band].vfo_b += tsu.band[tsu.curr_band].step;

			// Set request
			tsu.update_freq_dsp_req  = 1;

			// ToDo: Range check...
		}
	}
	else
	{
		if(tsu.band[tsu.curr_band].fixed_mode)
		{
			// ----------------------------------------
			// Limiter based on decoder mode
			if(tsu.band[tsu.curr_band].demod_mode == DEMOD_LSB)
			{
				if(tsu.band[tsu.curr_band].nco_freq > -20000)
					tsu.band[tsu.curr_band].nco_freq -= tsu.band[tsu.curr_band].step;
			}
			if(tsu.band[tsu.curr_band].demod_mode == DEMOD_USB)
			{
				if(tsu.band[tsu.curr_band].nco_freq > -24000)
					tsu.band[tsu.curr_band].nco_freq -= tsu.band[tsu.curr_band].step;
			}
			// Other modes handling...

			// Set request
			tsu.update_nco_dsp_req = 1;

			//printf("nco freq: %d\r\n",tsu.band[tsu.curr_band].nco_freq);
		}
		else
		{
			if(tsu.band[tsu.curr_band].active_vfo == 0)
				//tsu.vfo_a -= tsu.step;
				tsu.band[tsu.curr_band].vfo_a -= tsu.band[tsu.curr_band].step;
			else
				//tsu.vfo_b -= tsu.step;
				tsu.band[tsu.curr_band].vfo_b -= tsu.band[tsu.curr_band].step;

			// Set request
			tsu.update_freq_dsp_req  = 1;

			// ToDo: Range check...
		}
	}
	//printf("freq = %d\r\n", tsu.band[tsu.curr_band].vfo_a);

	// Save band info to eeprom
	save_band_info();
}

static void rotary_check_front_enc(void)
{
	ushort 	cnt;
	int		pot_diff = 0;

	if(front_init_done == 0)
		return;

	// No update on invalid local copy of the frequency
	if(tsu.band[tsu.curr_band].active_vfo == 0)
	{
		if(tsu.band[tsu.curr_band].vfo_a == 0xFFFFFFFF)
			return;
	}
	else
	{
		if(tsu.band[tsu.curr_band].vfo_b == 0xFFFFFFFF)
			return;
	}

	// No update on invalid local copy of the vfo step
	if(tsu.band[tsu.curr_band].step == 0xFFFFFFFF)
		return;

	cnt = __HAL_TIM_GET_COUNTER(&htim1);
	if(freq_old == cnt)
		return;

	//printf("---------------------------------\r\n");
	//printf("cnt = %d\r\n",cnt);

	if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim1))
		pot_diff = -1;
	else
		pot_diff = +1;

	//printf("pot_diff = %d\r\n",pot_diff);

	// Do not update publics while in Menu(or other non desktop modes, where UI is not painted and processed)
	if(ui_s.cur_state == MODE_DESKTOP)
		rotary_update_freq_publics(pot_diff);

	// Digital debounce - not working!
	//OsDelayMs(20);

	// Flag preventing calling too often
	freq_old = cnt;
}

uchar rotary_side_enc_init(void)
{
#if 0
	TIM_Encoder_InitTypeDef 	tim_config;
	TIM_MasterConfigTypeDef 	tim_master;
	GPIO_InitTypeDef 			GPIO_InitStruct;

	__HAL_RCC_TIM3_CLK_ENABLE();

	// PA7, TIM3, Q channel
	GPIO_InitStruct.Pin 				= GPIO_PIN_7;
	GPIO_InitStruct.Mode 				= GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull	 			= GPIO_PULLUP;
	GPIO_InitStruct.Speed 				= GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate 			= GPIO_AF2_TIM3;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	// PB4, TIM3, I channel
	GPIO_InitStruct.Pin 				= GPIO_PIN_4;
	GPIO_InitStruct.Alternate 			= GPIO_AF2_TIM3;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	htim3.Instance 						= TIM3;
	htim3.Init.Prescaler 				= 0;
	htim3.Init.CounterMode 				= TIM_COUNTERMODE_UP;
	htim3.Init.Period 					= TIM_PERIOD;
	htim3.Init.ClockDivision 			= TIM_CLOCKDIVISION_DIV4;
	//htim3.Init.AutoReloadPreload 		= TIM_AUTORELOAD_PRELOAD_DISABLE;
	tim_config.EncoderMode 				= TIM_ENCODERMODE_TI12;
	tim_config.IC1Polarity 				= TIM_ICPOLARITY_FALLING;
	tim_config.IC1Selection 			= TIM_ICSELECTION_DIRECTTI;
	tim_config.IC1Prescaler 			= TIM_ICPSC_DIV1;
	tim_config.IC1Filter 				= 0;
	tim_config.IC2Polarity 				= TIM_ICPOLARITY_FALLING;
	tim_config.IC2Selection 			= TIM_ICSELECTION_DIRECTTI;
	tim_config.IC2Prescaler 			= TIM_ICPSC_DIV1;
	tim_config.IC2Filter 				= 0;

	if (HAL_TIM_Encoder_Init(&htim3, &tim_config) != HAL_OK)
		return 1;

	tim_master.MasterOutputTrigger 		= TIM_TRGO_RESET;
	tim_master.MasterSlaveMode 			= TIM_MASTERSLAVEMODE_DISABLE;

	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &tim_master) != HAL_OK)
		return 2;

	if(HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_1 | TIM_CHANNEL_2) != HAL_OK)
		return 3;
#endif
	return 0;
}

uchar rotary_front_enc_init(void)
{
	TIM_Encoder_InitTypeDef 	tim_config;
	TIM_MasterConfigTypeDef 	tim_master;
	GPIO_InitTypeDef 			GPIO_InitStruct;

	__HAL_RCC_TIM1_CLK_ENABLE();

	// PK1, TIM1, CH1 (I channel)
	GPIO_InitStruct.Pin 				= GPIO_PIN_1;
	GPIO_InitStruct.Mode 				= GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull	 			= GPIO_PULLUP;
	GPIO_InitStruct.Speed 				= GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate 			= GPIO_AF1_TIM1;
	HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

	// PJ11, TIM1, CH2 (Q channel)
	GPIO_InitStruct.Pin 				= GPIO_PIN_11;
	GPIO_InitStruct.Alternate 			= GPIO_AF1_TIM1;
	HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);

	htim1.Instance 						= TIM1;
	htim1.Init.Prescaler 				= 0;
	htim1.Init.CounterMode 				= TIM_COUNTERMODE_UP;
	htim1.Init.Period 					= TIM_PERIOD;
	htim1.Init.ClockDivision 			= TIM_CLOCKDIVISION_DIV4;
	//htim3.Init.AutoReloadPreload 		= TIM_AUTORELOAD_PRELOAD_DISABLE;
	tim_config.EncoderMode 				= TIM_ENCODERMODE_TI12;
	tim_config.IC1Polarity 				= TIM_ICPOLARITY_FALLING;
	tim_config.IC1Selection 			= TIM_ICSELECTION_DIRECTTI;
	tim_config.IC1Prescaler 			= TIM_ICPSC_DIV1;
	tim_config.IC1Filter 				= 0;
	tim_config.IC2Polarity 				= TIM_ICPOLARITY_FALLING;
	tim_config.IC2Selection 			= TIM_ICSELECTION_DIRECTTI;
	tim_config.IC2Prescaler 			= TIM_ICPSC_DIV1;
	tim_config.IC2Filter 				= 0;

	if (HAL_TIM_Encoder_Init(&htim1, &tim_config) != HAL_OK)
		return 1;

	tim_master.MasterOutputTrigger 		= TIM_TRGO_RESET;
	tim_master.MasterSlaveMode 			= TIM_MASTERSLAVEMODE_DISABLE;

	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &tim_master) != HAL_OK)
		return 2;

	if(HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_1 | TIM_CHANNEL_2) != HAL_OK)
		return 3;

	HAL_NVIC_SetPriority(TIM1_TRG_COM_IRQn, 5 ,0U);
	HAL_NVIC_EnableIRQ	(TIM1_TRG_COM_IRQn);

	front_init_done = 1;

	return 0;
}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_ROTARY
//*----------------------------------------------------------------------------
static void rotary_worker(void)
{
	// Encoders
	//rotary_check_side_enc();
	rotary_check_front_enc();

	// Push buttons
	//rotary_check_side_encoder_switch();
}

// call from main() on startup
void rotary_proc_hw_init(void)
{
	// Encoders
	//rotary_side_enc_init();
	rotary_front_enc_init();

	// Push buttons
	//rotary_init_side_encoder_switch_pin();
}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_ROTARY
//*----------------------------------------------------------------------------
void rotary_proc(void const *arg)
{
	// Delay start, so UI can paint properly
	vTaskDelay(1000);
	printf("rotary driver start\r\n");

rotary_driver_loop:
	rotary_worker();
	vTaskDelay(100);
	goto rotary_driver_loop;
}

#endif




