/**
  ******************************************************************************
  * @file    settings_win.c
  * @author  MCD Application Team
  * @version V1.1.1
  * @date    18-November-2015
  * @brief   settings functions
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

#include "main.h"
#include "mchf_pro_board.h"

#ifdef CONTEXT_DRIVER_UI

#include "gui.h"
#include "dialog.h"
//#include "ST_GUI_Addons.h"

#include "k_module.h"

extern GUI_CONST_STORAGE GUI_BITMAP bmResetIcon;
  
static void Startup(WM_HWIN hWin, uint16_t xpos, uint16_t ypos);

K_ModuleItem_Typedef  reset =
{
  2,
  "Factory Reset",
  &bmResetIcon,
  Startup,
  NULL,
};

#define ID_WINDOW_0               	(GUI_ID_USER + 0x00)
#define ID_BUTTON_EXIT            	(GUI_ID_USER + 0x01)

#define ID_BUTTON_UI_RESET		  	(GUI_ID_USER + 0x02)
#define ID_BUTTON_DSP_RESET		  	(GUI_ID_USER + 0x03)
#define ID_BUTTON_EEP_RESET		  	(GUI_ID_USER + 0x04)

static const GUI_WIDGET_CREATE_INFO _aDialog[] = 
{
	// -----------------------------------------------------------------------------------------------------------------------------
	//							name					id						x		y		xsize	ysize	?		?		?
	// -----------------------------------------------------------------------------------------------------------------------------
	// Self
	{ WINDOW_CreateIndirect,	"", 					ID_WINDOW_0,			0,    	0,		800,	430, 	0, 		0x64, 	0 },
	// Back Button
	{ BUTTON_CreateIndirect, 	"Back",			 		ID_BUTTON_EXIT, 		670, 	375, 	120, 	45, 	0, 		0x0, 	0 },
	//
	{ BUTTON_CreateIndirect, 	"Restart UI",	 		ID_BUTTON_UI_RESET,		40, 	40, 	120, 	45, 	0, 		0x0, 	0 },
	{ BUTTON_CreateIndirect, 	"Restart DSP",	 		ID_BUTTON_DSP_RESET,	40, 	120, 	120, 	45, 	0, 		0x0, 	0 },
	{ BUTTON_CreateIndirect, 	"Reset Eeprom",	 		ID_BUTTON_EEP_RESET,	40, 	200, 	120, 	45, 	0, 		0x0, 	0 },
};

// API Driver messaging
//extern osMessageQId 					hApiMessage;
//struct APIMessage						api_reset;

static void _cbControl(WM_MESSAGE * pMsg, int Id, int NCode)
{
	WM_HWIN hItem;

	switch(Id)
	{
		// -------------------------------------------------------------
		// Button - exit
		case ID_BUTTON_EXIT:
		{
			switch(NCode)
			{
				case WM_NOTIFICATION_RELEASED:
					GUI_EndDialog(pMsg->hWin, 0);
					break;
			}
			break;
		}

		// -------------------------------------------------------------
		// Button - restart UI
		case ID_BUTTON_UI_RESET:
		{
			switch(NCode)
			{
				case WM_NOTIFICATION_RELEASED:
					NVIC_SystemReset();
					break;
			}
			break;
		}

		// -------------------------------------------------------------
		// Button - restart DSP
		case ID_BUTTON_DSP_RESET:
		{
			switch(NCode)
			{
				// Just set flag(or msg), to signal API driver, if DSP will honour the request, well that is another thing
				// This would reset both boards actually as UI power supply is coming from DSP controlled regulator
				case WM_NOTIFICATION_RELEASED:
				{
					// old via shared flag
					//--tsu.update_dsp_restart = 1;
					//
					// New implementation via message
//					api_reset.usMessageID 	= API_RESTART;
//					api_reset.ucPayload		= 0;									// payload count
//					osMessagePut(hApiMessage, (ulong)&api_reset, osWaitForever);
					break;
				}
			}
			break;
		}

		// -------------------------------------------------------------
		// Button - reset eeprom to default
		case ID_BUTTON_EEP_RESET:
		{
			switch(NCode)
			{
				case WM_NOTIFICATION_RELEASED:
				{
//					transceiver_init_eep_defaults();
					break;
				}
			}
			break;
		}

		// -------------------------------------------------------------
		default:
			break;
	}
}

static void _cbDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN 			hItem;
	int 				Id, NCode;

	switch (pMsg->MsgId)
	{
		case WM_INIT_DIALOG:
			break;

		case WM_PAINT:
			break;

		case WM_DELETE:
			break;

		case WM_NOTIFY_PARENT:
		{
			Id    = WM_GetId(pMsg->hWinSrc);    /* Id of widget */
			NCode = pMsg->Data.v;               /* Notification code */

			_cbControl(pMsg,Id,NCode);
			break;
		}

		// Process key messages not supported by ICON_VIEW control
		case WM_KEY:
		{
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key)
			{
		        // Return from menu
		        case GUI_KEY_HOME:
		        {
		        	//printf("GUI_KEY_HOME\r\n");
		        	GUI_EndDialog(pMsg->hWin, 0);
		        	break;
		        }
			}
			break;
		}

		default:
			WM_DefaultProc(pMsg);
			break;
	}
}

static void Startup(WM_HWIN hWin, uint16_t xpos, uint16_t ypos)
{
	GUI_CreateDialogBox(_aDialog, GUI_COUNTOF(_aDialog), _cbDialog, hWin, xpos, ypos);
}
#endif

