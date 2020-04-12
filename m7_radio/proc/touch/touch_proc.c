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

#include "main.h"
#include "mchf_pro_board.h"
#include "mchf_icc_def.h"

#include <limits.h>

#include "touch_proc.h"

#ifdef CONTEXT_TOUCH

#include "stm32h747i_discovery_audio.h"

osSemaphoreId TSSemaphoreID;

// Public radio state
extern struct	TRANSCEIVER_STATE_UI	tsu;

extern uint8_t BSP_Initialized;

void touch_proc_irq(void)
{
	if(TSSemaphoreID) osSemaphoreRelease(TSSemaphoreID);
}

//
// CONTEXT_RESET !!!!!!!
//
int touch_proc_hw_init(void)
{
	TS_Init_t hTS;
	uint8_t RetVal = 0;
	uint8_t counter = 10;

	hTS.Width = 800;
	hTS.Height = 480;
	hTS.Orientation = TS_SWAP_XY | TS_SWAP_Y;
	hTS.Accuracy = 5;

	do
	{
		RetVal = BSP_TS_Init(0, &hTS);
		HAL_Delay(100);
	    counter--;
	}
	while (counter && (RetVal != BSP_ERROR_NONE));

	if(RetVal != BSP_ERROR_NONE)
	{
		printf("Failed to initialize TS !! (Error %d)\n", RetVal);
	    return 1;
	}

	RetVal =  BSP_TS_EnableIT(0);
	if(RetVal != BSP_ERROR_NONE)
	{
		printf("Failed to initialize TS (IT) !! (Error %d)\n", RetVal);
	    return 2;
	}

	return 0;
}

static int touch_proc_claim_port(void)
{
	ulong timeout = 0xFFFFFF;

	// Wait to be free
	while(HAL_HSEM_IsSemTaken(HSEM_ID_22) == 1)
	{
		// Not possible to claim it
		if(timeout == 0)
			return 1;

		__asm("nop");
		timeout--;
	}

	// Take shared resource
	HAL_HSEM_Take(HSEM_ID_22, 0);

	return 0;
}

static void touch_proc_release_port(void)
{
	HAL_HSEM_Release(HSEM_ID_22, 0);
}

uint8_t touch_proc_ts_update(void)
{
	static GUI_PID_STATE TS_State = {0, 0, 0, 0};
	__IO TS_State_t  ts;
	uint16_t xDiff, yDiff;

	BSP_TS_GetState(0,(TS_State_t *)&ts);

	ts.TouchX = TouchScreen_Get_Calibrated_X(ts.TouchX);
	ts.TouchY = TouchScreen_Get_Calibrated_Y(ts.TouchY);

	if((ts.TouchX >= LCD_GetXSize()) ||(ts.TouchY >= LCD_GetYSize()) )
	{
		ts.TouchX = 0;
		ts.TouchY = 0;
	}

	xDiff = (TS_State.x > ts.TouchX) ? (TS_State.x - ts.TouchX) : (ts.TouchX - TS_State.x);
	yDiff = (TS_State.y > ts.TouchY) ? (TS_State.y - ts.TouchY) : (ts.TouchY - TS_State.y);

	if((TS_State.Pressed != ts.TouchDetected)||(xDiff > 8 )||(yDiff > 8))
	{
		TS_State.Pressed = ts.TouchDetected;
		TS_State.Layer = 0;

		if(ts.TouchDetected)
		{
			TS_State.x = ts.TouchX;
			TS_State.y = ts.TouchY ;
			GUI_TOUCH_StoreStateEx(&TS_State);
		}
		else
		{
			GUI_TOUCH_StoreStateEx(&TS_State);
			TS_State.x = 0;
			TS_State.y = 0;
		}

		return 1;
	}

	return 0;
}

void touch_proc_exec(ulong cmd)
{
	//printf("audio command req: %02x\r\n", cmd);
	switch(cmd)
	{
		// Mute Audio
		case 1:
			BSP_AUDIO_OUT_Mute(0);
			break;

		// Un-mute Audio
		case 2:
			BSP_AUDIO_OUT_UnMute(0);
			break;

		// Update volume
		case 3:
			BSP_AUDIO_OUT_SetVolume(0, tsu.band[tsu.curr_band].volume);
			break;

		default:
			break;
	}
}

void touch_proc_task(void const *argument)
{
	uint32_t 	ulNotificationValue = 0, ulNotif;

	//
	// Core to core HW semaphore - ToDo: do we need if not using IRQ ???
	//
	//--HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_22));

	osSemaphoreDef(TSSemaphore);
	TSSemaphoreID = osSemaphoreCreate(osSemaphore(TSSemaphore), 1);

	// Audio init
	BSP_AUDIO_INOUT_InitA();

	// initially take the TS Lock
	osSemaphoreWait( TSSemaphoreID, osWaitForever );

	while(1)
	{
		// ------------------------------------------------------------------
		// Wait touch IRQ
		if((BSP_Initialized == 1) && (osSemaphoreWait( TSSemaphoreID, 1) == 0))
		{
			// Sync with DSP core
			if(touch_proc_claim_port() == 0)
			{
				touch_proc_ts_update();
				touch_proc_release_port();
			}
		}

		// ------------------------------------------------------------------
		// Wait notification
		ulNotif = xTaskNotifyWait(0x00, ULONG_MAX, &ulNotificationValue, 1);
		if((ulNotif) && (ulNotificationValue))
		{
			// Sync with DSP core
			if(touch_proc_claim_port() == 0)
			{
				touch_proc_exec(ulNotificationValue);
				touch_proc_release_port();
			}
		}
  }
}
#endif

