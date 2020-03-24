/************************************************************************************
**                                                                                 **
**                             mcHF Pro QRP Transceiver                            **
**                         Krassi Atanassov - M0NKA 2012-2019                      **
**                            mail: djchrismarc@gmail.com                          **
**                                 twitter: @M0NKA_                                **
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

#ifndef __API_DSP_H
#define __API_DSP_H

#ifdef DSP_MODE

// Command interface
#define API_UPD_FREQ					0x0001
#define API_UPD_BAND					0x0002
#define API_UPD_VOL						0x0003
#define API_UPD_DEMOD					0x0004
#define API_UPD_FILT					0x0005
#define API_UPD_STEP					0x0006
#define API_UPD_NCO						0x0007
#define API_CW_TX						0x0008
#define API_BROADCAST_MODE				0x0009
#define API_WRITE_EEP					0x000A
#define API_RESTART						0x000B

// ----------------------------------------------------------------
#define API_DMA_MODE

#define SPI2_DMA                       	DMA1

#define SPI2_TX_DMA_CHANNEL            	DMA_Channel_0
#define SPI2_TX_DMA_STREAM             	DMA1_Stream4
#define SPI2_TX_DMA_FLAG_TCIF          	DMA_FLAG_TCIF4
#define SPI2_TX_DMA_IRQ          		DMA1_Stream4_IRQn

#define SPI2_RX_DMA_CHANNEL            	DMA_Channel_0
#define SPI2_RX_DMA_STREAM            	DMA1_Stream3
#define SPI2_RX_DMA_FLAG_TCIF          	DMA_FLAG_TCIF3
#define SPI2_RX_DMA_IRQ          		DMA1_Stream3_IRQn

// Exports
void api_dsp_init(void);
void api_dsp_thread(void);
void api_dsp_irq(void);

void api_dsp_post(q15_t *fft);

void api_dsp_systick(void);

#endif

#endif
