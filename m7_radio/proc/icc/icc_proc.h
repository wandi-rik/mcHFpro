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
#ifndef __ICC_PROC_H
#define __ICC_PROC_H

#define API_UI_ALLOW_DEBUG

#define API_TRANSFER_SIZE				300
#define API_DMA_RX_B_SIZE				64

// -----------------------------------------------------------------------
// DMA transfers - still work in progress
//#define	API_DRIVER_USE_DMA

#ifdef API_DRIVER_USE_DMA
//#define	API_DRIVER_DMA_FULL_DUPLEX
#define	API_DRIVER_DMA_RX_ONLY
//#define	API_DRIVER_DMA_TX_ONLY
//
#define SPI2_TX_DMA_STREAM               DMA1_Stream6
#define SPI2_TX_DMA_REQUEST              DMA_REQUEST_SPI2_TX
#define SPI2_DMA_TX_IRQn                 DMA1_Stream6_IRQn
//
#define SPI2_RX_DMA_STREAM               DMA1_Stream5
#define SPI2_RX_DMA_REQUEST              DMA_REQUEST_SPI2_RX
#define SPI2_DMA_RX_IRQn                 DMA1_Stream5_IRQn
#endif

// -----------------------------------------------------------------------

void api_proc_hw_init(void);
void icc_proc_task(void const * argument);

#endif
