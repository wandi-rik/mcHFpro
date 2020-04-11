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

#define ID_LISTBOX1           		(GUI_ID_USER + 0x02)
#define ID_LISTBOX2           		(GUI_ID_USER + 0x03)

static const GUI_WIDGET_CREATE_INFO _aDialog[] = 
{
	// -----------------------------------------------------------------------------------------------------------------------------
	//							name						id					x		y		xsize	ysize	?		?		?
	// -----------------------------------------------------------------------------------------------------------------------------
	// Self
	{ WINDOW_CreateIndirect,	"", 						ID_WINDOW_0,		0,    	0,		800,	430, 	0, 		0x64, 	0 },
	// Back Button
	{ BUTTON_CreateIndirect, 	"Back",			 			ID_BUTTON_EXIT, 	670, 	375, 	120, 	45, 	0, 		0x0, 	0 },
	// List boxes
	{ LISTBOX_CreateIndirect, 	"Listbox",					ID_LISTBOX1, 		  5, 	 10, 	520, 	410, 	0, 		0x0, 	0 },
	{ LISTBOX_CreateIndirect, 	"Listbox",					ID_LISTBOX2, 		535, 	 10, 	255, 	350, 	0, 		0x0, 	0 },
};

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

static void about_print_fw_vers(WM_HWIN hItem)
{
	char fw_id[200];
	char *p = fw_id;

    memset(fw_id,0,sizeof(fw_id));
    strcpy(p,"Device ID:");
    p += strlen("Device ID:");
    strcpy(p," ");
    p++;
    strcpy(p,DEVICE_STRING);
    p += strlen(DEVICE_STRING);
    sprintf(p," v %d.%d.%d.%d",MCHF_R_VER_MAJOR, MCHF_R_VER_MINOR, MCHF_R_VER_RELEASE,MCHF_R_VER_BUILD);

    LISTBOX_AddString(hItem,fw_id);
}

static void about_print_fw_auth(WM_HWIN hItem)
{
	char fw_id[200];
	char *p = fw_id;

    memset(fw_id,0,sizeof(fw_id));
    strcpy(p,AUTHOR_STRING);

    LISTBOX_AddString(hItem, fw_id);
    LISTBOX_SetSel(hItem, -1);
}

static void about_print_fw_rtos(WM_HWIN hItem)
{
	char fw_id[200];
	char *p = fw_id;

    memset(fw_id,0,sizeof(fw_id));
    sprintf(p,"%s FreeRTOS %s","Scheduler:",tskKERNEL_VERSION_NUMBER);

    LISTBOX_AddString(hItem, fw_id);
}

static void about_print_fw_gui(WM_HWIN hItem)
{
	char fw_id[200];
	char *p = fw_id;

    memset(fw_id,0,sizeof(fw_id));
    sprintf(p,"%s emWin v %s","UI GuiLib:",GUI_GetVersionString());

    LISTBOX_AddString(hItem, fw_id);
}

static void about_print_fw_dsp(WM_HWIN hItem)
{
	char fw_id[200];

    memset(fw_id,0,sizeof(fw_id));
    sprintf(fw_id,"DSP v: %d.%d.%d.%d",tsu.dsp_rev1,tsu.dsp_rev2,tsu.dsp_rev3,tsu.dsp_rev4);

    LISTBOX_AddString(hItem, fw_id);
}

#if 0
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
#endif

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
	WM_HWIN 	hList;
	char 		buf[200];

	// Is it enabled ?
	if(state_id == 0xFF)
		return;

	hList = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX1);
	LISTBOX_SetSel(hList, -1);

	// Caller finished ?
	if(esp_msg.ucProcStatus == TASK_PROC_DONE)
	{
		state_id++;									// next state
		esp_msg.ucProcStatus = TASK_PROC_IDLE;		// reset
	}
	else if(esp_msg.ucProcStatus == TASK_PROC_WORK)
		return;

	//--printf("state id: %d\r\n", state_id);

	// State machine
	switch(state_id)
	{
		// Radio firmware version
		case 0:
		{
			about_print_fw_vers(hList);
			state_id++;
			break;
		}

		// DSP firmware version
		case 1:
		{
			about_print_fw_dsp(hList);
			state_id++;
			break;
		}

		// OS version
		case 2:
		{
			about_print_fw_rtos(hList);
			state_id++;
			break;
		}

		// OS version
		case 3:
		{
			about_print_fw_gui(hList);
			state_id++;
			break;
		}

		// CPU Speed
		case 4:
		{
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "CPU: %d MHz, PCLK1: %d MHz, PCLK2: %d MHz",	(int)(HAL_RCCEx_GetD1SysClockFreq()/1000000U),
			                                           					(int)(HAL_RCC_GetPCLK1Freq()/1000000U),
																		(int)(HAL_RCC_GetPCLK2Freq()/1000000U));
			LISTBOX_AddString(hList, buf);
			state_id++;
			break;
		}

		// ESP32 Firmware version request
		case 5:
		{
			esp_msg.ucMessageID = 0x01;
			esp_msg.ucProcStatus = TASK_PROC_WORK;
			osMessagePut(hEspMessage, (ulong)&esp_msg, osWaitForever);
			break;
		}

		// Result from previous state
		case 6:
		{
			if((esp_msg.ucExecResult == 0) && (esp_msg.ucDataReady))
			{
				//printf("ui: %s\r\n", (char *)(esp_msg.ucData));
				memset(buf, 0, sizeof(buf));
				sprintf(buf,  "ESP32 firmware: %s", (char *)(esp_msg.ucData + 10));
				LISTBOX_AddString(hList, buf);
			}
			state_id++;
			break;
		}

		// ESP32 WiFi details
		case 7:
		{
			esp_msg.ucMessageID  = 0x02;
			esp_msg.ucProcStatus = TASK_PROC_WORK;
			osMessagePut(hEspMessage, (ulong)&esp_msg, osWaitForever);
			break;
		}

		// Result from previous state
		case 8:
		{
			if((esp_msg.ucExecResult == 0) && (esp_msg.ucDataReady) && (esp_msg.ucData[9]))
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

				memset(buf, 0, sizeof(buf));
				sprintf(buf, "SSID: %s", (char *)(&esp_msg.ucData[21]));
				LISTBOX_AddString(hList, buf);
			}

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
	//SCROLLBAR_Handle 	hScrollV;

	switch (pMsg->MsgId)
	{
		case WM_INIT_DIALOG:
		{
			hList = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX2);
			LISTBOX_SetFont(hList, &GUI_Font16B_ASCII);
			about_print_fw_auth(hList);

			hList = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX1);
			LISTBOX_SetFont(hList, &GUI_Font24B_ASCII);
			//LISTBOX_SetTextColor(hList,LISTBOX_CI_UNSEL|LISTBOX_CI_SEL|LISTBOX_CI_SELFOCUS|LISTBOX_CI_DISABLED,GUI_BLUE);

			// Start the state machine
			hTimerListFill = WM_CreateTimer(pMsg->hWin, 0, LIST_TIMER_RESOLUTION, 0);
			esp_msg.ucProcStatus = TASK_PROC_IDLE;
			state_id  = 0;

			break;
		}

		case WM_TIMER:
		{
			// Next line
			info_state_machine(pMsg);

			// Update state
			if(state_id < 9)
				WM_RestartTimer(pMsg->Data.v, LIST_TIMER_RESOLUTION);

			break;
		}

		case WM_PAINT:
			break;

		case WM_DELETE:
			WM_DeleteTimer(hTimerListFill);
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
