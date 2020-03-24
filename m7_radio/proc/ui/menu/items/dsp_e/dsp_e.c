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

#include "gui.h"
#include "dialog.h"
//#include "ST_GUI_Addons.h"

//#include "aa_res.c"
#include "k_module.h"

extern GUI_CONST_STORAGE GUI_BITMAP bmicon_test;
  
static void Startup(WM_HWIN hWin, uint16_t xpos, uint16_t ypos);

K_ModuleItem_Typedef  dsp_e =
{
  5,
  "Extended DSP Menu",
  &bmicon_test,
  Startup,
  NULL,
};

#define ID_WINDOW_0               (GUI_ID_USER + 0x00)
#define ID_BUTTON_EXIT            (GUI_ID_USER + 0x01)

static const GUI_WIDGET_CREATE_INFO _aDialog[] = 
{
	// -----------------------------------------------------------------------------------------------------------------------------
	//							name						id					x		y		xsize	ysize	?		?		?
	// -----------------------------------------------------------------------------------------------------------------------------
	// Self
	{ WINDOW_CreateIndirect,	"", 						ID_WINDOW_0,		0,    	0,		800,	430, 	0, 		0x64, 	0 },
	// Back Button
	{ BUTTON_CreateIndirect, 	"Back",			 			ID_BUTTON_EXIT, 	670, 	375, 	120, 	45, 	0, 		0x0, 	0 },
};

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

