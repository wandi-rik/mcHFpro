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

#include "ui_controls_wifi.h"
#include "desktop\ui_controls_layout.h"

// Public radio state
extern struct	TRANSCEIVER_STATE_UI	tsu;

static const GUI_WIDGET_CREATE_INFO WiFiDialog[] =
{
	// -----------------------------------------------------------------------------------------------------------------------------
	//							name		id					x		y		xsize				ysize				?		?		?
	// -----------------------------------------------------------------------------------------------------------------------------
	// Self
	{ WINDOW_CreateIndirect,	"", 		ID_WINDOW_WIFI,		0,		0,		WIFI_SIZE_X,		WIFI_SIZE_Y, 		0, 		0x64, 	0 },
	//
	{ TEXT_CreateIndirect, 		"----",		ID_TEXT_WIFI,		1,		1,		78, 				13,  				0, 		0x0,	0 },
};

WM_HWIN 	hWiFiDialog;

#define	WIFI_TIMER_RESOLUTION	2000
//
WM_HTIMER 						hTimerWiFi;

static void WDHandler(WM_MESSAGE *pMsg)
{
	WM_HWIN hItem;
	int 	Id, NCode;

	switch (pMsg->MsgId)
	{
		case WM_INIT_DIALOG:
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_WIFI);
			TEXT_SetFont(hItem,&GUI_Font16B_1);
			TEXT_SetBkColor(hItem,GUI_LIGHTBLUE);
			TEXT_SetTextColor(hItem,GUI_WHITE);
			TEXT_SetTextAlign(hItem,TEXT_CF_RIGHT|TEXT_CF_VCENTER);

			hTimerWiFi = WM_CreateTimer(pMsg->hWin, 0, WIFI_TIMER_RESOLUTION, 0);
			break;
		}

		case WM_TIMER:
		{
			if(tsu.wifi_rssi)
			{
				char buf[30];
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_WIFI);
				sprintf(buf, "%d dBm", tsu.wifi_rssi);
				TEXT_SetText(hItem, buf);
			}

//!			WM_InvalidateWindow(hWiFiDialog);
			WM_RestartTimer(pMsg->Data.v, WIFI_TIMER_RESOLUTION);
			break;
		}

		case WM_PAINT:
			break;

		case WM_DELETE:
			WM_DeleteTimer(hTimerWiFi);
			break;

		case WM_NOTIFY_PARENT:
		{
			Id    = WM_GetId(pMsg->hWinSrc);    // Id of widget
			NCode = pMsg->Data.v;               // Notification code

			//VDCHandler(pMsg,Id,NCode);
			break;
		}

		// Trap keyboard messages
		case WM_KEY:
		{
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key)
			{
		        // Return from menu
		        case GUI_KEY_HOME:
		        {
		        	//printf("GUI_KEY_HOME\r\n");
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

//*----------------------------------------------------------------------------
//* Function Name       : ui_controls_wifi_init
//* Object              :
//* Notes    			: create control
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_wifi_init(WM_HWIN hParent)
{
	hWiFiDialog = GUI_CreateDialogBox(WiFiDialog, GUI_COUNTOF(WiFiDialog), WDHandler, hParent, WIFI_X, WIFI_Y);
}

//*----------------------------------------------------------------------------
//* Function Name       : ui_controls_wifi_quit
//* Object              :
//* Notes    			: clean-up
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_wifi_quit(void)
{
	GUI_EndDialog(hWiFiDialog, 0);
}

#endif
