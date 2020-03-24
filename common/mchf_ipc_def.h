/************************************************************************************
**                                                                                 **
**                             mcHF Pro QRP Transceiver                            **
**                         Krassi Atanassov - M0NKA 2012-2020                      **
**                            mail: djchrismarc@gmail.com                          **
**                                 twitter: @bph_co                                **
**---------------------------------------------------------------------------------**
**                                                                                 **
**  File name:                                                                     **
**  Description:      ESP32 <-> STM32 Inter process communication                  **
**  Last Modified:                                                                 **
**  Licence:                                                                       **
**          The mcHF project is released for radio amateurs experimentation,       **
**          non-commercial use only. All source files under GPL-3.0, unless        **
**          third party drivers specifies otherwise. Thank you!                    **
************************************************************************************/

#ifndef __MCHF_IPC_DEF_H
#define __MCHF_IPC_DEF_H

#define MENU_READ_ESP_32_VERSION       		0x10
#define MENU_0X01 							0x01
#define MENU_0X02 							0x02
#define MENU_0X03 							0x03
#define MENU_0X04 							0x04
#define MENU_0X05 							0x05
#define MENU_WIFI_GET_DETAILS 				0x06
#define MENU_0X07 							0x07
#define MENU_0X08 							0x08
#define MENU_0X09 							0x09
#define MENU_0X0A 							0x0A
#define MENU_WIFI_SCAN_START 				0x20
#define MENU_WIFI_GET_AVAILABLE_NETWORKS 	0x21
#define MENU_WIFI_GET_NETWORK_DETAILS 		0x22
#define MENU_WIFI_CONNECT_TO_NETWORK 		0x23
#define MENU_WIFI_GET_CONNECTION_STATUS 	0x24
#define MENU_WIFI_GET_NETWORK_RSSI 			0x25
#define MENU_READ_ROM_MAC 					0x30
#define MENU_READ_ROM_BANK1 				0x31
#define MENU_READ_ROM_BANK2 				0x32
#define MENU_READ_ROM_BANK3 				0x33
#define MENU_SET_SOAP_SERVER 				0x40

#define MENU_ESP32_REBOOT 					0xF0


#endif
