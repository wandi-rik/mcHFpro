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
#include "main.h"

#ifdef CONTEXT_DRIVER_UI

#include "gui.h"
#include "dialog.h"
//#include "ST_GUI_Addons.h"

#include "ui_controls_vfo_step.h"
#include "desktop\ui_controls_layout.h"

#include "stm32h7xx_hal_gpio.h"

// Public radio state
extern struct	TRANSCEIVER_STATE_UI	tsu;

extern ulong tune_steps[];

ulong loc_step = 0;

static void ui_controls_vfo_init_encoder_switch_pin(void)
{
#if 0
	GPIO_InitTypeDef  	GPIO_InitStruct;

	GPIO_InitStruct.Pin 		= GPIO_PIN_12;
	GPIO_InitStruct.Mode 		= GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull 		= GPIO_PULLUP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif
}

void ui_controls_vfo_step_update_step(void)
{
	ulong step = tsu.band[tsu.curr_band].step;

	// No need to update if nothing changed
	if(step == loc_step)
		return;

	//printf("control step %d\r\n",tsu.step);

	GUI_SetFont(&GUI_Font8x16_1);
	GUI_SetColor(GUI_BLACK);

	switch(step)
	{
		case T_STEP_1HZ:
			GUI_FillRoundedRect((VFO_STEP_X + 1),(VFO_STEP_Y + 1),(VFO_STEP_X + VFO_STEP_SIZE_X - 1),(VFO_STEP_Y + VFO_STEP_SIZE_Y - 1),2);
			GUI_SetColor(GUI_ORANGE);
			GUI_DispStringAt("1Hz",(VFO_STEP_X + 16),(VFO_STEP_Y + 3));
			break;
		case T_STEP_10HZ:
			GUI_FillRoundedRect((VFO_STEP_X + 1),(VFO_STEP_Y + 1),(VFO_STEP_X + VFO_STEP_SIZE_X - 1),(VFO_STEP_Y + VFO_STEP_SIZE_Y - 1),2);
			GUI_SetColor(GUI_ORANGE);
			GUI_DispStringAt("10Hz",(VFO_STEP_X + 10),(VFO_STEP_Y + 3));
			break;
		case T_STEP_100HZ:
			GUI_FillRoundedRect((VFO_STEP_X + 1),(VFO_STEP_Y + 1),(VFO_STEP_X + VFO_STEP_SIZE_X - 1),(VFO_STEP_Y + VFO_STEP_SIZE_Y - 1),2);
			GUI_SetColor(GUI_ORANGE);
			GUI_DispStringAt("100Hz",(VFO_STEP_X + 8),(VFO_STEP_Y + 3));
			break;
		case T_STEP_1KHZ:
			GUI_FillRoundedRect((VFO_STEP_X + 1),(VFO_STEP_Y + 1),(VFO_STEP_X + VFO_STEP_SIZE_X - 1),(VFO_STEP_Y + VFO_STEP_SIZE_Y - 1),2);
			GUI_SetColor(GUI_ORANGE);
			GUI_DispStringAt("1kHz",(VFO_STEP_X + 12),(VFO_STEP_Y + 3));
			break;
		case T_STEP_10KHZ:
			GUI_FillRoundedRect((VFO_STEP_X + 1),(VFO_STEP_Y + 1),(VFO_STEP_X + VFO_STEP_SIZE_X - 1),(VFO_STEP_Y + VFO_STEP_SIZE_Y - 1),2);
			GUI_SetColor(GUI_ORANGE);
			GUI_DispStringAt("10kHz",(VFO_STEP_X + 8),(VFO_STEP_Y + 3));
			break;
		case T_STEP_100KHZ:
			GUI_FillRoundedRect((VFO_STEP_X + 1),(VFO_STEP_Y + 1),(VFO_STEP_X + VFO_STEP_SIZE_X - 1),(VFO_STEP_Y + VFO_STEP_SIZE_Y - 1),2);
			GUI_SetColor(GUI_ORANGE);
			GUI_DispStringAt("100kHz",(VFO_STEP_X + 3),(VFO_STEP_Y + 3));
			break;
		case T_STEP_1MHZ:
			GUI_FillRoundedRect((VFO_STEP_X + 1),(VFO_STEP_Y + 1),(VFO_STEP_X + VFO_STEP_SIZE_X - 1),(VFO_STEP_Y + VFO_STEP_SIZE_Y - 1),2);
			GUI_SetColor(GUI_ORANGE);
			GUI_DispStringAt("1MHz",(VFO_STEP_X + 13),(VFO_STEP_Y + 3));
			break;
		case T_STEP_10MHZ:
			GUI_FillRoundedRect((VFO_STEP_X + 1),(VFO_STEP_Y + 1),(VFO_STEP_X + VFO_STEP_SIZE_X - 1),(VFO_STEP_Y + VFO_STEP_SIZE_Y - 1),2);
			GUI_SetColor(GUI_ORANGE);
			GUI_DispStringAt("10MHz",(VFO_STEP_X + 8),(VFO_STEP_Y + 3));
			break;
		default:
			break;
	}

	// Save for next call, to prevent over repaint
	loc_step = step;
}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_vfo_step_init(void)
{
	loc_step = 0;

	// If using the frequency encoder switch
	ui_controls_vfo_init_encoder_switch_pin();

	GUI_SetColor(GUI_ORANGE);
	GUI_DrawRoundedRect((VFO_STEP_X + 0),(VFO_STEP_Y + 0),(VFO_STEP_X + VFO_STEP_SIZE_X),(VFO_STEP_Y + VFO_STEP_SIZE_Y),2);
	GUI_SetFont(&GUI_Font8x16_1);
	//GUI_DispStringAt("1kHz",(VFO_STEP_X + 10),(VFO_STEP_Y + 3));

	//printf("on control init - dsp step %d\r\n",tsu.dsp_step);
	//printf("on control init - tsu step %d\r\n",tsu.step);

//!	tsu.step = tune_steps[tsu.dsp_step_idx];
	ui_controls_vfo_step_update_step();
}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_vfo_step_quit(void)
{

}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_vfo_step_touch(void)
{
	// nothing
}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_vfo_step_refresh(void)
{
#if 0
	// Encoder button clicked ?
	if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12))
	{
		while(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12));

		//printf("encoder clicked\r\n");

		// toggle 100 Hz and 1 khz
		if(tsu.band[tsu.curr_band].step != T_STEP_100HZ)
		{
			tsu.band[tsu.curr_band].step = T_STEP_100HZ;
			//tsu.step_idx = 2;
		}
		else
		{
			tsu.band[tsu.curr_band].step = T_STEP_1KHZ;
			//tsu.step_idx = 3;
		}
	}
#endif

	//printf("dsp step %d\r\n",tsu.dsp_step);
	//printf("on control refresh - tsu step %d\r\n",tsu.step);

	ui_controls_vfo_step_update_step();

}

#endif
