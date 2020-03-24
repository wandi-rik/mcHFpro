
#include "mchf_pro_board.h"

#include "gui.h"
#include "dialog.h"
//#include "ST_GUI_Addons.h"

#include "k_module.h"
#include "ui_menu.h"

// ---------------------------------------------------------
// ST Animated control or standard emWin IconView
//
#define	USE_ANIMATED_ICON_VIEW
//
// ---------------------------------------------------------

#define ID_ICONVIEW_MENU               	(GUI_ID_USER + 0x00)
#define ID_MENU_NAME                	(GUI_ID_USER + 0xD0)

ICONVIEW_Handle 						hIcon;
//WM_HWIN         						hCPULoad;
WM_HWIN   								hItem;

uchar									main_repaint_done;

static void ui_menu_update_footer_text(char *text)
{
	WM_HWIN   	hItem;

	hItem = WM_GetDialogItem(WM_GetDesktopWindowEx(0), ID_MENU_NAME);
	if(!hItem)
		return;

	if(!WM_IsVisible(hItem))
		return;

	if(text == NULL)
		return;

	// strlen check ??
	// ...

	GUI_SetColor(GUI_ORANGE);
	GUI_FillRect(FOOTER_EDIT_X, FOOTER_EDIT_Y,(FOOTER_EDIT_X + FOOTER_EDIT_X_SIZE),(FOOTER_EDIT_Y + FOOTER_EDIT_Y_SIZE - 2));

	TEXT_SetText(hItem,text);

	WM_InvalidateWindow(hItem);
}

#ifdef USE_ANIMATED_ICON_VIEW
static void _cbBk(WM_MESSAGE * pMsg)
{
	uint32_t NCode, Id;
	static uint8_t sel;

	switch (pMsg->MsgId)
	{
		// Process key messages not supported by ICON_VIEW control
		case WM_KEY:
		{
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key)
			{
				// Enter menu
		        case GUI_KEY_ENTER:
		        {
		        	printf("GUI_KEY_ENTER\r\n");

		        	//sel = ST_AnimatedIconView_GetSel(pMsg->hWinSrc);
		        	sel = ICONVIEW_GetSel(pMsg->hWinSrc);
		        	if(sel < k_ModuleGetNumber())
		        	{
		        		//ST_AnimatedIconView_SetSel(pMsg->hWinSrc, -1);
		        		ICONVIEW_SetSel(pMsg->hWinSrc, -1);
		        		if(module_prop[sel].in_use == 0)
		        		{
		        			// Update footer with menu name
		        			ui_menu_update_footer_text((char *)module_prop[sel].module->name);

		        			module_prop[sel].in_use = 1;
		        			module_prop[sel].module->startup(pMsg->hWin, 0, 0);
		        		}
		        		else if(module_prop[sel].win_state == 1)
		        		{
		        			module_prop[sel].module->startup(pMsg->hWin, 0, 0);
		        		}
		        	}
		        	else
		        	{
		        		WM_InvalidateWindow (pMsg->hWinSrc);
		        	}

		        	break;
		        }
			}
			break;
		}

		case WM_PAINT:
		{
			if(!main_repaint_done)
			{
				//printf("WM_PAINT, initial\r\n");

				// Menu window back colour
				GUI_SetBkColor(GUI_WHITE);
				GUI_Clear();

				// Create footer
				GUI_SetColor(GUI_ORANGE);
				GUI_FillRoundedRect(5, 435, 795, 475, 10);

				WM_SetFocus(hIcon);

				// lock repaint
				main_repaint_done = 1;
			}

			break;
		}

		case WM_NOTIFY_PARENT:
		{
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;

			//printf("WM_NOTIFY_PARENT\r\n");

			switch (NCode)
			{
				case WM_NOTIFICATION_RELEASED:
				{
					if(Id == ID_ICONVIEW_MENU)
					{
						//printf("icon enter\r\n");

						//sel = ST_AnimatedIconView_GetSel(pMsg->hWinSrc);
						sel = ICONVIEW_GetSel(pMsg->hWinSrc);
						if(sel < k_ModuleGetNumber())
						{
							//ST_AnimatedIconView_SetSel(pMsg->hWinSrc, -1);
							ICONVIEW_SetSel(pMsg->hWinSrc, -1);
							if(module_prop[sel].in_use == 0)
							{
								// Update footer with menu name
								ui_menu_update_footer_text((char *)module_prop[sel].module->name);

								module_prop[sel].in_use = 1;
								module_prop[sel].module->startup(pMsg->hWin, 0, 0);
							}
							else if(module_prop[sel].win_state == 1)
							{
								module_prop[sel].module->startup(pMsg->hWin, 0, 0);
							}
						}
						else
						{
							WM_InvalidateWindow (pMsg->hWinSrc);
						}
					}
					break;
				}

				case WM_NOTIFICATION_CHILD_DELETED:
				{
					//printf("WM_NOTIFICATION_CHILD_DELETED\r\n");

					// Restore footer name
					ui_menu_update_footer_text("Main Menu");

					// Allow screen repaint (once)
					main_repaint_done = 0;

					module_prop[sel].in_use = 0;
					module_prop[sel].win_state = 0;
					break;
				}

				default:
					break;
			}

			break;
		}

		default:
			WM_DefaultProc(pMsg);
	}
}
#endif

void ui_periodic_processes(void)
{
#if 0
	char 		tmp[16];
	WM_HWIN   	hItem;
	static 		uint32_t InitTick = 0;

	hItem = WM_GetDialogItem(WM_GetDesktopWindowEx(0), ID_FEATURES_CPU);

	if(hItem)
	{
		if((WM_IsVisible(hItem)) && ((osKernelSysTick() - InitTick ) > 500))
		{
			InitTick = osKernelSysTick();

			EnterCriticalSection();
			sprintf((char *)tmp   , "CPU Load : %d%%",  osGetCPUUsage());
			ExitCriticalSection();

			// Print new
			TEXT_SetText(hItem, tmp);

			WM_InvalidateWindow(hItem);
			WM_Update(hItem);
		}
	}
#endif
}

void ui_set_gui_profile(void)
{
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
	DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
	MENU_SetDefaultSkin(MENU_SKIN_FLEX);

	CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);
	SPINBOX_SetDefaultSkin(SPINBOX_SKIN_FLEX);

    FRAMEWIN_SetDefaultTextColor(0, GUI_WHITE);
    FRAMEWIN_SetDefaultTextColor(1, GUI_WHITE);

    FRAMEWIN_SetDefaultBarColor(0, GUI_LIGHTBLUE);
    FRAMEWIN_SetDefaultBarColor(1, GUI_LIGHTBLUE);

    FRAMEWIN_SetDefaultFont(&GUI_Font24_1);

    FRAMEWIN_SetDefaultClientColor(GUI_WHITE);
    FRAMEWIN_SetDefaultTitleHeight(25);

    WINDOW_SetDefaultBkColor(GUI_WHITE);

    // ---------------------------------------------------------------------------
    LISTVIEW_SetDefaultGridColor(GUI_WHITE);
    LISTVIEW_SetDefaultBkColor(LISTVIEW_CI_SEL, 	 	GUI_LIGHTBLUE);
    LISTVIEW_SetDefaultBkColor(LISTVIEW_CI_SELFOCUS,	GUI_LIGHTBLUE);
    LISTVIEW_SetDefaultTextColor(LISTVIEW_CI_UNSEL,		GUI_DARKBLUE);
    LISTVIEW_SetDefaultFont(&GUI_Font16_1);
    //
    //--HEADER_SetDefaultBkColor(GUI_STCOLOR_LIGHTBLUE);
    HEADER_SetDefaultTextColor(GUI_LIGHTBLUE);
    HEADER_SetDefaultFont(&GUI_Font24_1);
    //--HEADER_SetDefaultSTSkin();
    // ---------------------------------------------------------------------------

    DROPDOWN_SetDefaultColor(DROPDOWN_CI_SEL, GUI_LIGHTBLUE);
    DROPDOWN_SetDefaultColor(DROPDOWN_CI_SELFOCUS, GUI_LIGHTBLUE);

    TREEVIEW_SetDefaultFont(GUI_FONT_24B_ASCII);
    TREEVIEW_SetDefaultTextColor(TREEVIEW_CI_UNSEL,GUI_DARKBLUE);

    //ST_CHOOSEFILE_SetDelim('/');
    GUI_SetDefaultFont(GUI_FONT_20_ASCII);

    //ST_CHOOSEFILE_SetButtonSize(40, 20);

  	TEXT_SetDefaultTextColor(GUI_LIGHTBLUE);
  	TEXT_SetDefaultFont(&GUI_Font20_1);

  	// List box defaults
  	LISTBOX_SetDefaultScrollMode(LISTBOX_CF_AUTOSCROLLBAR_V);

  	// Need extra large buttons for Clock menu
  	SPINBOX_SetDefaultButtonSize(64);

  	// Allow scroll bar on a list box
  	SCROLLBAR_SetDefaultColor(GUI_LIGHTBLUE, SCROLLBAR_CI_THUMB|SCROLLBAR_CI_SHAFT|SCROLLBAR_CI_ARROW);
  	SCROLLBAR_SetDefaultWidth(50);
  	//--SCROLLBAR_SetDefaultSTSkin();

  	//RADIO_SetDefaultFont(&GUI_FontAvantGarde16);
  	RADIO_SetDefaultFocusColor(GUI_LIGHTBLUE);
  	RADIO_SetDefaultTextColor(GUI_LIGHTBLUE);

  	CALENDAR_SetDefaultSize(CALENDAR_SI_HEADER, 50);

  	CALENDAR_SetDefaultSize(CALENDAR_SI_CELL_X, 65);
  	CALENDAR_SetDefaultSize(CALENDAR_SI_CELL_Y, 45);

  	CALENDAR_SetDefaultFont(CALENDAR_FI_CONTENT,GUI_FONT_24_1 );
  	CALENDAR_SetDefaultFont(CALENDAR_FI_HEADER, GUI_FONT_24_1);
}

#ifdef USE_ANIMATED_ICON_VIEW
void ui_init_menu(void)
{
	uint8_t i = 0;

	main_repaint_done = 0;

    // Create desktop
    WM_SetCallback(WM_GetDesktopWindowEx(0), _cbBk);

    //hIcon = ST_AnimatedIconView_CreateEx(	0,
    hIcon = ICONVIEW_CreateEx( 0,
    										0,
											LCD_GetXSize() - 0,
											LCD_GetYSize() - 0,
											WM_GetDesktopWindowEx(0),
											WM_CF_SHOW | WM_CF_HASTRANS | WM_CF_LATE_CLIP,
											0,
											ID_ICONVIEW_MENU,
											200,										// xsize items
											143											// ysize items
											);

    //ST_AnimatedIconView_SetDualFont(hIcon, &GUI_FontAvantGarde20, &GUI_FontAvantGarde20);
    ICONVIEW_SetFont(hIcon, &GUI_Font20_1);

    //ST_AnimatedIconView_SetSpace(hIcon, GUI_COORD_Y, 0);
    ICONVIEW_SetSpace(hIcon, GUI_COORD_Y, 0);
    //ST_AnimatedIconView_SetSpace(hIcon, GUI_COORD_X, 0);
    ICONVIEW_SetSpace(hIcon, GUI_COORD_X, 0);

    //ST_AnimatedIconView_SetFrame(hIcon, GUI_COORD_Y, 0);
    ICONVIEW_SetFrame(hIcon, GUI_COORD_Y, 0);
    //ST_AnimatedIconView_SetFrame(hIcon, GUI_COORD_X, 0);
    ICONVIEW_SetFrame(hIcon, GUI_COORD_X, 0);

    //ST_AnimatedIconView_SetTextColor(hIcon, 	ICONVIEW_CI_UNSEL,		GUI_BLUE);
    ICONVIEW_SetTextColor(hIcon, 	ICONVIEW_CI_UNSEL,		GUI_BLUE);
    //ST_AnimatedIconView_SetTextColor(hIcon, 	ICONVIEW_CI_SEL,	 	GUI_RED);
	ICONVIEW_SetTextColor(hIcon, 	ICONVIEW_CI_SEL,	 	GUI_RED);

    //ST_AnimatedIconView_SetBkColor(hIcon, ICONVIEW_CI_UNSEL, GUI_WHITE);
	ICONVIEW_SetBkColor(hIcon, ICONVIEW_CI_UNSEL, GUI_WHITE);
    //ST_AnimatedIconView_SetBkColor(hIcon, ICONVIEW_CI_SEL, GUI_WHITE);
	ICONVIEW_SetBkColor(hIcon, ICONVIEW_CI_SEL, GUI_WHITE);

    //ST_AnimatedIconView_SetSel(hIcon, -1);
	ICONVIEW_SetSel(hIcon, -1);
    //ST_AnimatedIconView_SetDualTextColor(hIcon, ICONVIEW_CI_SEL, GUI_LIGHTBLUE, GUI_DARKBLUE);

    for (i = 0; i < k_ModuleGetNumber(); i++)
    {
    	//ST_AnimatedIconView_AddIcon(hIcon, module_prop[i].module->open_icon, module_prop[i].module->close_icon, (char *)module_prop[i].module->name);
    	ICONVIEW_AddBitmapItem(hIcon, module_prop[i].module->icon, (char *)module_prop[i].module->name);
    }

    // Create footer text control
    hItem = TEXT_CreateEx(FOOTER_EDIT_X, FOOTER_EDIT_Y, FOOTER_EDIT_X_SIZE, FOOTER_EDIT_Y_SIZE, WM_GetDesktopWindowEx(0), WM_CF_SHOW, TEXT_CF_LEFT, ID_MENU_NAME, "Main Menu");
    TEXT_SetFont(hItem, &GUI_Font24B_1);
    TEXT_SetTextColor(hItem, GUI_WHITE);

    WM_SetFocus(hIcon);
}
#endif

void ui_destroy_menu(void)
{
	WM_DeleteWindow(hIcon);
	WM_DeleteWindow(hItem);
	WM_SetCallback		(WM_HBKWIN, 0);
	WM_InvalidateWindow	(WM_HBKWIN);
}

