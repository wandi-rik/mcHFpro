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

#ifndef __IPC_PROC_H
#define __IPC_PROC_H

#ifdef CONTEXT_IPC_PROC

// The IPC protocol is well defined in common/mchf_ipf_def.h
// file, including return payload size (which is variable)
// So we have two options here on RX DMA - use pre-defined
// payload sizes from the .h file or do DMA with full
// buffer size, then abort the transfer when LINE_IDLE IRQ
// fires (which is somewhat challenging on ST Cortex chips)
//
//#define IPC_USE_IDLE_LINE

/* Definition for USARTx's DMA */
#define USARTx_TX_DMA_STREAM             DMA1_Stream7
#define USARTx_RX_DMA_STREAM             DMA1_Stream1

/* Definition for USARTx's DMA Request */
#define USARTx_TX_DMA_REQUEST            DMA_REQUEST_UART8_TX
#define USARTx_RX_DMA_REQUEST            DMA_REQUEST_UART8_RX

/* Definition for USARTx's NVIC */
#define USARTx_DMA_TX_IRQn               DMA1_Stream7_IRQn
#define USARTx_DMA_RX_IRQn               DMA1_Stream1_IRQn

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      UART8_IRQn

void ipc_proc_init(void);
void ipc_proc_task(void const *arg);

#endif

#endif
