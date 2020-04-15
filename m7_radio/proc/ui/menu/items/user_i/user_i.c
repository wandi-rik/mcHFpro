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

#include "mchf_pro_board.h"

#include "gui.h"
#include "dialog.h"
//#include "ST_GUI_Addons.h"

//#include "aa_res.c"
#include "k_module.h"

extern GUI_CONST_STORAGE GUI_BITMAP bmicon_consumer;

//const GUI_BITMAP * user_i_anim[] = {
//  &bmicon_consumer,   &bmicon_consumer,   &bmicon_consumer,   &bmicon_consumer, &bmicon_consumer
//};
  
static void Startup(WM_HWIN hWin, uint16_t xpos, uint16_t ypos);

K_ModuleItem_Typedef  user_i =
{
  6,
  "User Interface",
  &bmicon_consumer,
  Startup,
  NULL,
};

#define ID_WINDOW_0               	(GUI_ID_USER + 0x00)
#define ID_BUTTON_EXIT            	(GUI_ID_USER + 0x01)

#define ID_CHECKBOX_0				(GUI_ID_USER + 0x02)
#define ID_CHECKBOX_1				(GUI_ID_USER + 0x03)
#define ID_CHECKBOX_2				(GUI_ID_USER + 0x04)

static const GUI_WIDGET_CREATE_INFO _aDialog[] = 
{
	// -----------------------------------------------------------------------------------------------------------------------------
	//							name						id					x		y		xsize	ysize	?		?		?
	// -----------------------------------------------------------------------------------------------------------------------------
	// Self
	{ WINDOW_CreateIndirect,	"", 						ID_WINDOW_0,		0,    	0,		800,	430, 	0, 		0x64, 	0 },
	// Back Button
	{ BUTTON_CreateIndirect, 	"Back",			 			ID_BUTTON_EXIT, 	670, 	375, 	120, 	45, 	0, 		0x0, 	0 },
	// Check boxes
	{ CHECKBOX_CreateIndirect,	"Checkbox", 				ID_CHECKBOX_0, 		20, 	 20,	250, 	30, 	0, 		0x0, 	0 },
	{ CHECKBOX_CreateIndirect,	"Checkbox", 				ID_CHECKBOX_1, 		20, 	 70,	250, 	30, 	0, 		0x0, 	0 },
	{ CHECKBOX_CreateIndirect,	"Checkbox", 				ID_CHECKBOX_2, 		20, 	120,	250, 	30, 	0, 		0x0, 	0 },
};

extern 	osMessageQId 			hEspMessage;
struct 	ESPMessage				esp_msg;

static void _cbControl(WM_MESSAGE * pMsg, int Id, int NCode)
{
	WM_HWIN hItem;

	switch(Id)
	{
		// -------------------------------------------------------------
		// Button
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

		// ------------------------------------------------------------
		//
		case ID_CHECKBOX_0:
		{
			switch(NCode)
		    {
				case WM_NOTIFICATION_CLICKED:
					break;
				case WM_NOTIFICATION_RELEASED:
					break;
				case WM_NOTIFICATION_VALUE_CHANGED:
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);
					// Save to eeprom
					// *(uchar *)(EEP_BASE + EEP_SW_SMOOTH) = CHECKBOX_GetState(hItem);

					if(esp_msg.ucProcStatus == TASK_PROC_IDLE)
					{
						esp_msg.ucMessageID  = 0x04;			// SQLite write
						esp_msg.ucProcStatus = TASK_PROC_WORK;

						esp_msg.ucData[0] = (uchar)CHECKBOX_GetState(hItem);

						strcpy((char *)(esp_msg.ucData + 5), "sd_smooth");
						esp_msg.ucData[4] = 10;

						esp_msg.usPayloadSize = 15;

						osMessagePut(hEspMessage, (ulong)&esp_msg, osWaitForever);
					}

					break;
				}
				default:
					break;
		    }
			break;
		}

		// ------------------------------------------------------------
		//
		case ID_CHECKBOX_1:
		{
			switch(NCode)
		    {
				case WM_NOTIFICATION_CLICKED:
					break;
				case WM_NOTIFICATION_RELEASED:
					break;
				case WM_NOTIFICATION_VALUE_CHANGED:
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1);
					// Save to eeprom
					*(uchar *)(EEP_BASE + EEP_AN_MET_ON) = CHECKBOX_GetState(hItem);

					break;
				}
				default:
					break;
		    }
			break;
		}

		// ------------------------------------------------------------
		//
		case ID_CHECKBOX_2:
		{
			switch(NCode)
		    {
				case WM_NOTIFICATION_CLICKED:
					break;
				case WM_NOTIFICATION_RELEASED:
					break;
				case WM_NOTIFICATION_VALUE_CHANGED:
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2);
					// Save to eeprom
					*(uchar *)(EEP_BASE + EEP_KEYER_ON) = CHECKBOX_GetState(hItem);

					break;
				}
				default:
					break;
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
		{
			// Init Exit button
			//hItem = BUTTON_CreateEx(695, 375, 100, 60, pMsg->hWin, WM_CF_SHOW, 0, ID_BUTTON_EXIT);
			//WM_SetCallback(hItem, _cbButton_exit);

			// Init Checkbox
			hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);
			CHECKBOX_SetFont(hItem,&GUI_Font16_1);
			CHECKBOX_SetText(hItem, "Spectrum Display Smooth Mode");
			CHECKBOX_SetState(hItem, *(uchar *)(EEP_BASE + EEP_SW_SMOOTH));

			// Init Checkbox
			hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1);
			CHECKBOX_SetFont(hItem,&GUI_Font16_1);
			CHECKBOX_SetText(hItem, "Enable Analogue S-Meter");
			CHECKBOX_SetState(hItem, *(uchar *)(EEP_BASE + EEP_AN_MET_ON));

			// Init Checkbox
			hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2);
			CHECKBOX_SetFont(hItem,&GUI_Font16_1);
			CHECKBOX_SetText(hItem, "Show Iambic Keyer Control");
			CHECKBOX_SetState(hItem, *(uchar *)(EEP_BASE + EEP_KEYER_ON));
    
			esp_msg.ucProcStatus = TASK_PROC_IDLE;
			break;
		}

		case WM_PAINT:
			break;

		case WM_DELETE:
			break;

		case WM_NOTIFY_PARENT:
		{
			Id    = WM_GetId(pMsg->hWinSrc);    // Id of widget
			NCode = pMsg->Data.v;               // Notification code

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

