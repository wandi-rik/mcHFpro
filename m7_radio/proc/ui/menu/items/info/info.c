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
#include "version.h"

#ifdef CONTEXT_DRIVER_UI

#include "gui.h"
#include "dialog.h"
#include "FreeRTOS.h"
#include "task.h"

//#include "aa_res.c"
#include "k_module.h"

// temp, from digitizer driver
extern uchar digitizer_info[];
  
// Core unique regs loaded to RAM
extern struct	CM7_CORE_DETAILS	ccd;

static void Startup(WM_HWIN hWin, uint16_t xpos, uint16_t ypos);

// Public radio state
extern struct	TRANSCEIVER_STATE_UI	tsu;

extern GUI_CONST_STORAGE GUI_BITMAP bmgeneralinfoA;

K_ModuleItem_Typedef  info =
{
  0,
  "System Information",
  &bmgeneralinfoA,
  Startup,
  NULL,
};

#define ID_WINDOW_0               	(GUI_ID_USER + 0x00)
#define ID_BUTTON_EXIT            	(GUI_ID_USER + 0x01)

#define ID_TEXT_UI_HDR             	(GUI_ID_USER + 0x03)
#define ID_TEXT_LO_HDR            	(GUI_ID_USER + 0x04)

#define ID_TEXT_UI_FW_V           	(GUI_ID_USER + 0x05)
#define ID_TEXT_UI_FW_A           	(GUI_ID_USER + 0x06)
#define ID_TEXT_UI_OS_V           	(GUI_ID_USER + 0x07)
#define ID_TEXT_UI_GUI_V           	(GUI_ID_USER + 0x08)
#define ID_TEXT_UI_TS_V           	(GUI_ID_USER + 0x09)
#define ID_TEXT_UI_CPU_ID          	(GUI_ID_USER + 0x0A)
#define ID_TEXT_UI_FLS_SZ          	(GUI_ID_USER + 0x0B)

#define ID_TEXT_LO_PCB_V           	(GUI_ID_USER + 0x0C)

static const GUI_WIDGET_CREATE_INFO _aDialog[] = 
{
	// -----------------------------------------------------------------------------------------------------------------------------
	//							name						id					x		y		xsize	ysize	?		?		?
	// -----------------------------------------------------------------------------------------------------------------------------
	// Self
	{ WINDOW_CreateIndirect,	"", 						ID_WINDOW_0,		0,    	0,		800,	430, 	0, 		0x64, 	0 },
	// Back Button
	{ BUTTON_CreateIndirect, 	"Back",			 			ID_BUTTON_EXIT, 	670, 	375, 	120, 	45, 	0, 		0x0, 	0 },
	// Board headers
	{ TEXT_CreateIndirect, 		"User Interface", 			ID_TEXT_UI_HDR,		120,	10,		300,	40,		0,		0x0,	0 },
	{ TEXT_CreateIndirect, 		"Logic Board", 				ID_TEXT_LO_HDR,		530,	10,		300,	40,		0,		0x0,	0 },
	// UI board entries
	{ TEXT_CreateIndirect, 		"Firm.Vers", 				ID_TEXT_UI_FW_V,	10,		 50,	360,	20,		0,		0x0,	0 },
	{ TEXT_CreateIndirect, 		"Firm.Auth", 				ID_TEXT_UI_FW_A,	10,		 66,	360,	20,		0,		0x0,	0 },
	{ TEXT_CreateIndirect, 		"RTOS.Vers", 				ID_TEXT_UI_OS_V,	10,		 82,	360,	20,		0,		0x0,	0 },
	{ TEXT_CreateIndirect, 		"RTOS.Vers", 				ID_TEXT_UI_GUI_V,	10,		 98,	360,	20,		0,		0x0,	0 },
	{ TEXT_CreateIndirect, 		"Digi.Vers", 				ID_TEXT_UI_TS_V,	10,		114,	360,	20,		0,		0x0,	0 },
	{ TEXT_CreateIndirect, 		"Cpu.ID",	 				ID_TEXT_UI_CPU_ID,	10,		130,	360,	20,		0,		0x0,	0 },
	{ TEXT_CreateIndirect, 		"Flash.Size", 				ID_TEXT_UI_FLS_SZ,	10,		146,	360,	20,		0,		0x0,	0 },
	// Logic board entries
	{ TEXT_CreateIndirect, 		"Logic.PCB", 				ID_TEXT_LO_PCB_V,	420,	 50,	360,	20,		0,		0x0,	0 },
};

#define	TEXT_1					"Device ID:"
#define	TEXT_2					"Author ID:"
#define	TEXT_3					"Scheduler:"
#define	TEXT_4					"UI GuiLib:"
#define	TEXT_5					"Digitizer:"
#define	TEXT_6					"Unique ID:"
#define	TEXT_7					"FlashSize:"

#define	TEXT_8					"Logic PCB:"

static void about_print_fw_vers(WM_HWIN hItem)
{
	char fw_id[200];
	char *p = fw_id;

	EnterCriticalSection();
    memset(fw_id,0,sizeof(fw_id));
    strcpy(p,TEXT_1);
    p += strlen(TEXT_1);
    strcpy(p," ");
    p++;
    strcpy(p,DEVICE_STRING);
    p += strlen(DEVICE_STRING);
    sprintf(p," v %d.%d.%d.%d",MCHF_R_VER_MAJOR, MCHF_R_VER_MINOR, MCHF_R_VER_RELEASE,MCHF_R_VER_BUILD);
    ExitCriticalSection();

    p = fw_id;

    TEXT_SetText(hItem,fw_id);
}

static void about_print_fw_auth(WM_HWIN hItem)
{
	char fw_id[200];
	char *p = fw_id;

	EnterCriticalSection();
    memset(fw_id,0,sizeof(fw_id));
    strcpy(p,TEXT_2);
    p += strlen(TEXT_2);
    strcpy(p," ");
    p++;
    strcpy(p,AUTHOR_STRING);
    p += strlen(AUTHOR_STRING);
    ExitCriticalSection();

    p = fw_id;

    TEXT_SetText(hItem,fw_id);
}

static void about_print_fw_rtos(WM_HWIN hItem)
{
	char fw_id[200];
	char *p = fw_id;

	EnterCriticalSection();
    memset(fw_id,0,sizeof(fw_id));
    //sprintf(p,"Switch: FreeRTOS %s",tskKERNEL_VERSION_NUMBER);
    sprintf(p,"%s FreeRTOS %s",TEXT_3,tskKERNEL_VERSION_NUMBER);
    ExitCriticalSection();

    p = fw_id;

    TEXT_SetText(hItem,fw_id);
}

static void about_print_fw_gui(WM_HWIN hItem)
{
	char fw_id[200];
	char *p = fw_id;

	EnterCriticalSection();
    memset(fw_id,0,sizeof(fw_id));
    //sprintf(p,"GuiLib: emWin v %s",GUI_GetVersionString());
    sprintf(p,"%s emWin v %s",TEXT_4,GUI_GetVersionString());
    ExitCriticalSection();

    p = fw_id;

    TEXT_SetText(hItem,fw_id);
}

static void about_print_fw_digi(WM_HWIN hItem)
{
	char fw_id[200];
	char *p = fw_id;

	EnterCriticalSection();
    memset(fw_id,0,sizeof(fw_id));
    //sprintf(p,"Digitz: FT5206%s",digitizer_info);
 //   sprintf(p,"%s FT5206%s",TEXT_5,digitizer_info);
    ExitCriticalSection();

    p = fw_id;

    TEXT_SetText(hItem,fw_id);
}

static void about_print_fw_cpu_id(WM_HWIN hItem)
{
	char fw_id[200];
	char *p = fw_id;

	// It would seems only byte access to the Unique ID reg works
	//uchar *p_cpu = (uchar *)0x1FF0F420;
	//ulong a,b,c;

	// Keep reads outside of the IRQ blocking, to prevent OS stalling
	// in case of buggy cpu hw
	//a = *(p + 0x00) | (*(p + 0x01) << 8) | (*(p + 0x02) << 16) | (*(p + 0x03) << 24);
	//b = *(p + 0x04) | (*(p + 0x05) << 8) | (*(p + 0x06) << 16) | (*(p + 0x07) << 24);
	//c = *(p + 0x08) | (*(p + 0x09) << 8) | (*(p + 0x0A) << 16) | (*(p + 0x0B) << 24);

	EnterCriticalSection();
    memset(fw_id,0,sizeof(fw_id));
    //sprintf(p,"Cpu ID: %08x-%08x-%08x",a,b,c);
    //sprintf(p,"Cpu ID: %08x-%08x-%08x",ccd.wafer_coord,ccd.wafer_number,ccd.lot_number);
 //   sprintf(p,"%s %08x-%08x-%08x",TEXT_6,ccd.wafer_coord,ccd.wafer_number,ccd.lot_number);
    ExitCriticalSection();

    p = fw_id;

    TEXT_SetText(hItem,fw_id);
}

static void about_print_fw_fls_sz(WM_HWIN hItem)
{
	char fw_id[200];
	char *p = fw_id;

	EnterCriticalSection();
    memset(fw_id,0,sizeof(fw_id));
    //sprintf(p,"F size: %dkB",ccd.fls_size);
 //   sprintf(p,"%s %dkB",TEXT_7,ccd.fls_size);
    ExitCriticalSection();

    p = fw_id;

    TEXT_SetText(hItem,fw_id);
}

static void about_print_pcb_rev(WM_HWIN hItem)
{
	char fw_id[200];
	char *p = fw_id;

	EnterCriticalSection();
    memset(fw_id,0,sizeof(fw_id));
    strcpy(p,TEXT_8);
    p += strlen(TEXT_8);
    strcpy(p," ");
    p++;
//    sprintf(p,"rev 0.%d",tsu.pcb_rev);
    ExitCriticalSection();

    p = fw_id;

    TEXT_SetText(hItem,fw_id);
}

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
	SCROLLBAR_Handle 	hScrollV;

	switch (pMsg->MsgId)
	{
		case WM_INIT_DIALOG:
		{
			// Init Exit button
			//hItem = BUTTON_CreateEx(695, 375, 100, 60, pMsg->hWin, WM_CF_SHOW, 0, ID_BUTTON_EXIT);
			//WM_SetCallback(hItem, _cbButton_exit);

			// Format Text
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_UI_HDR);
			TEXT_SetFont(hItem,&GUI_Font32_1);
			//
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_LO_HDR);
			TEXT_SetFont(hItem,&GUI_Font32_1);

			// Print firmware info
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_UI_FW_V);
			TEXT_SetFont(hItem,&GUI_Font8x16_1);
			TEXT_SetTextColor(hItem, GUI_BLACK);
			about_print_fw_vers(hItem);
			//
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_UI_FW_A);
			TEXT_SetFont(hItem,&GUI_Font8x16_1);
			TEXT_SetTextColor(hItem, GUI_BLACK);
			about_print_fw_auth(hItem);
			//
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_UI_OS_V);
			TEXT_SetFont(hItem,&GUI_Font8x16_1);
			TEXT_SetTextColor(hItem, GUI_BLACK);
			about_print_fw_rtos(hItem);
			//
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_UI_GUI_V);
			TEXT_SetFont(hItem,&GUI_Font8x16_1);
			TEXT_SetTextColor(hItem, GUI_BLACK);
			about_print_fw_gui(hItem);
			//
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_UI_TS_V);
			TEXT_SetFont(hItem,&GUI_Font8x16_1);
			TEXT_SetTextColor(hItem, GUI_BLACK);
			about_print_fw_digi(hItem);
			//
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_UI_CPU_ID);
			TEXT_SetFont(hItem,&GUI_Font8x16_1);
			TEXT_SetTextColor(hItem, GUI_BLACK);
			about_print_fw_cpu_id(hItem);
			//
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_UI_FLS_SZ);
			TEXT_SetFont(hItem,&GUI_Font8x16_1);
			TEXT_SetTextColor(hItem, GUI_BLACK);
			about_print_fw_fls_sz(hItem);

			// Print firmware info
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_LO_PCB_V);
			TEXT_SetFont(hItem,&GUI_Font8x16_1);
			TEXT_SetTextColor(hItem, GUI_BLACK);
			about_print_pcb_rev(hItem);

			break;
		}

		case WM_PAINT:
		{
			// Text frames
			GUI_SetColor(GUI_ORANGE);
			GUI_DrawRoundedRect(  5,  5,395,360,10);
			GUI_DrawRoundedRect(405,  5,790,360,10);

			break;
		}

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

#endif
