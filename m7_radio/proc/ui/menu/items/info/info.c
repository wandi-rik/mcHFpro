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
//extern uchar digitizer_info[];
  
// Core unique regs loaded to RAM
//extern struct	CM7_CORE_DETAILS	ccd;

static void Startup(WM_HWIN hWin, uint16_t xpos, uint16_t ypos);

// Public radio state
//extern struct	TRANSCEIVER_STATE_UI	tsu;

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
#define ID_LISTBOX1           		(GUI_ID_USER + 0x0D)

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
//	{ TEXT_CreateIndirect, 		"Logic.PCB", 				ID_TEXT_LO_PCB_V,	420,	 50,	360,	20,		0,		0x0,	0 },
	{ LISTBOX_CreateIndirect, 	"Listbox",					ID_LISTBOX1, 		405, 	 10, 	380, 	350, 	0, 		0x0, 	0 },
};

#define	TEXT_1					"Device ID:"
#define	TEXT_2					"Author ID:"
#define	TEXT_3					"Scheduler:"
#define	TEXT_4					"UI GuiLib:"
#define	TEXT_5					"Digitizer:"
#define	TEXT_6					"Unique ID:"
#define	TEXT_7					"FlashSize:"

#define	TEXT_8					"Logic PCB:"

// ------------------------------------------------------
//
// List update state machine
//
#define	LIST_TIMER_RESOLUTION	100
//
WM_HTIMER 						hTimerListFill;
//
extern 	osMessageQId 			hEspMessage;
struct 	ESPMessage				esp_msg;
//
ulong 							state_id  = 0xFF;
int								last_item = 0;


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

static void info_state_machine(WM_MESSAGE * pMsg)
{
	WM_HWIN 	hItem, hList;
	char 		buf[200];

	// Is it enabled ?
	if(state_id == 0xFF)
		return;

	// Caller finished ?
	if(esp_msg.ucProcStatus == TASK_PROC_DONE)
	{
		state_id++;									// next state
		esp_msg.ucProcStatus = TASK_PROC_IDLE;		// reset
	}
	else if(esp_msg.ucProcStatus == TASK_PROC_WORK)
		return;

	// State machine
	switch(state_id)
	{
#if 0
		// Project details
		case 20:
		{
			memset(buf, 0, sizeof(buf));
			sprintf(buf,"%s v %d.%d.%d.%d - %s", DEVICE_STRING,
											MCHF_L_VER_MAJOR,
											MCHF_L_VER_MINOR,
											MCHF_L_VER_RELEASE,
											MCHF_L_VER_BUILD,
											AUTHOR_STRING);

			hList = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX1);
			LISTBOX_AddString(hList, buf);
			//LISTBOX_SetSel(hList, -1);
			last_item++;

			// Update progress bar
			hItem = WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_0);
			PROGBAR_SetValue(hItem, 10);

			state_id++;
			break;
		}
#endif

		// CPU Speed
		case 0:
		{
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "CPU: %d MHz, PCLK1: %d MHz, PCLK2: %d MHz",(int)(HAL_RCCEx_GetD1SysClockFreq()/1000000U),
			                                           				(int)(HAL_RCC_GetPCLK1Freq()/1000000U),
																(int)(HAL_RCC_GetPCLK2Freq()/1000000U));
			hList = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX1);
			LISTBOX_AddString(hList, buf);
			//LISTBOX_SetSel(hList, -1);
			//last_item++;

			// Update progress bar
			//hItem = WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_0);
			//PROGBAR_SetValue(hItem, 10);

			state_id++;
			break;
		}

		// ESP32 Firmware version request
		case 1:
		{
			//hList = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX1);
			//LISTBOX_AddString(hList, "Reading ESP32 firmware version..." );
			//LISTBOX_SetSel(hList, -1);

			// Request UART call
			esp_msg.ucMessageID = 0x01;
			esp_msg.ucProcStatus = TASK_PROC_WORK;
			if(osMessagePut(hEspMessage, (ulong)&esp_msg, osWaitForever) != osOK)
			{
				state_id = 0xFF;
				return;
			}

			break;
		}

		// Result from previous state
		case 2:
		{
			hList = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX1);
			//LISTBOX_DeleteItem(hList, last_item);

			if(esp_msg.ucExecResult == 0)
			{
				//LISTBOX_AddString(hList, "Reading ESP32 firmware version...Success" );
				if(esp_msg.ucDataReady)
				{
					printf("ui: %s\r\n", (char *)(esp_msg.ucData));

					memset(buf, 0, sizeof(buf));
					sprintf(buf,  "ESP32 firmware: %s", (char *)(esp_msg.ucData + 10));
					LISTBOX_AddString(hList, buf);
					last_item++;
				}
			}
			else
			{
				//memset(buf, 0, sizeof(buf));
				//sprintf(buf,  "Reading ESP32 firmware version...Error %d", esp_msg.ucExecResult);
				//LISTBOX_AddString(hList, buf);

				state_id = 0xFF;
				return;
			}
			//LISTBOX_SetSel(hList, -1);
			last_item++;

			state_id++;
			break;
		}

		// ESP32 WiFi details
		case 3:
		{
			hList = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX1);
			//LISTBOX_AddString(hList, "Reading WiFi details..." );
			//LISTBOX_SetSel(hList, -1);

			// Update progress bar
			//hItem = WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_0);
			//PROGBAR_SetValue(hItem, 40);

			// Request UART call
			esp_msg.ucMessageID = 0x02;
			esp_msg.ucProcStatus = TASK_PROC_WORK;
			if(osMessagePut(hEspMessage, (ulong)&esp_msg, osWaitForever) != osOK)
			{
				state_id = 0xFF;
				return;
			}

			break;
		}

		// Result from previous state
		case 4:
		{
			hList = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX1);
			//LISTBOX_DeleteItem(hList, last_item);

			if(esp_msg.ucExecResult == 0)
			{
				//LISTBOX_AddString(hList, "Reading WiFi details...Success" );
				if(esp_msg.ucDataReady)
				{
					printf("[gui] data size: %d\r\n", esp_msg.ucDataReady);
					if(esp_msg.ucData[9])
					{
						memset(buf, 0, sizeof(buf));
						sprintf(buf,  "MAC: %02x:%02x:%02x:%02x:%02x:%02x, IP: %d.%d.%d.%d", 	esp_msg.ucData[10],
																								esp_msg.ucData[11],
																								esp_msg.ucData[12],
																								esp_msg.ucData[13],
																								esp_msg.ucData[14],
																								esp_msg.ucData[15],
																								esp_msg.ucData[16],
																								esp_msg.ucData[17],
																								esp_msg.ucData[18],
																								esp_msg.ucData[19]);

						LISTBOX_AddString(hList, buf);
						//last_item++;

						memset(buf, 0, sizeof(buf));
						sprintf(buf, "SSID: %s", (char *)(&esp_msg.ucData[21]));
						LISTBOX_AddString(hList, buf);
					}
				}
			}
			else
			{
				//memset(buf, 0, sizeof(buf));
				//sprintf(buf,  "Reading WiFi details...Error %d", esp_msg.ucExecResult);
				//LISTBOX_AddString(hList, buf);

				state_id = 0xFF;
				return;
			}

			//last_item++;

			// Update progress bar
			//hItem = WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_0);
			//PROGBAR_SetValue(hItem, 50);

			state_id++;
			break;
		}

		default:
			break;
	}
}

static void _cbDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN 			hItem;
	int 				Id, NCode;
	WM_HWIN 			hList;
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

			hList = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX1);
			LISTBOX_SetFont(hList, &GUI_Font8x16_1);
			//LISTBOX_SetTextColor(hList,LISTBOX_CI_UNSEL|LISTBOX_CI_SEL,GUI_LIGHTBLUE);

			// Start the state machine
			hTimerListFill = WM_CreateTimer(pMsg->hWin, 0, LIST_TIMER_RESOLUTION, 0);
			esp_msg.ucProcStatus = TASK_PROC_IDLE;
			state_id  = 0;
			last_item = 0;

			break;
		}

		case WM_TIMER:
		{
			//WM_InvalidateWindow(pMsg->hWin);
			info_state_machine(pMsg);			// next state
			WM_RestartTimer(pMsg->Data.v, LIST_TIMER_RESOLUTION);
			break;
		}

		case WM_PAINT:
		{
			// Text frames
			GUI_SetColor(GUI_ORANGE);
			GUI_DrawRoundedRect(  5,  5,395,360,10);
			//GUI_DrawRoundedRect(405,  5,790,360,10);
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
