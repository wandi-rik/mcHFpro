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
#include "bsp.h"

#include "ipc_proc.h"

#include "wifi.h"
#include "mchf_ipc_def.h"

#ifdef CONTEXT_IPC_PROC

UART_HandleTypeDef 	UART8_Handle;

__IO ITStatus 		UartReady 		= RESET;
__IO ITStatus 		UartError 		= RESET;
__IO ITStatus 		UartLineIdle 	= RESET;

DMA_HandleTypeDef 	hdma_tx;
DMA_HandleTypeDef	hdma_rx;

extern QueueHandle_t 	hEspMessage;

__attribute__((section("heap_mem"))) ALIGN_32BYTES (uint8_t TxBuffer[128]);
__attribute__((section("heap_mem"))) ALIGN_32BYTES (uint8_t RxBuffer[1056]);	// [10b header][1024b payload][1b checksum], cache aligned

void UART8_IRQHandler(void)
{
	#ifdef IPC_USE_IDLE_LINE
	//if (UART8_Handle.Instance->ISR & USART_ISR_IDLE)
	if(RESET != __HAL_UART_GET_FLAG(&UART8_Handle, UART_FLAG_IDLE))
	{
		UartLineIdle = SET;

		volatile uint32_t tmp;
	    tmp = UART8_Handle.Instance->ISR;
	    tmp = UART8_Handle.Instance->RDR;

	    //__HAL_UART_CLEAR_IDLEFLAG(&UART8_Handle);
	    //HAL_UART_DMAStop(&UART8_Handle);
	    //((DMA_Stream_TypeDef *)hdma_rx.Instance)->CR &= ~DMA_SxCR_EN;
	}
	#endif

	HAL_UART_IRQHandler(&UART8_Handle);
}

void DMA1_Stream1_IRQHandler(void)
{
	HAL_DMA_IRQHandler(UART8_Handle.hdmarx);
}

void DMA1_Stream7_IRQHandler(void)
{
	HAL_DMA_IRQHandler(UART8_Handle.hdmatx);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	UartReady = SET;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	UartReady = SET;
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
	if( UartHandle->ErrorCode == HAL_UART_ERROR_ORE)
	{
		__HAL_UART_CLEAR_OREFLAG(UartHandle);
		UartHandle->ErrorCode |= HAL_UART_ERROR_ORE;
	}
	else
		UartError = SET;
}

//*----------------------------------------------------------------------------
//* Function Name       : esp32_uart_init
//* Object              :
//* Notes    			: called from main(), not task!
//* Notes    			:
//* Context    			: CONTEXT_RESET_VECTOR
//*----------------------------------------------------------------------------
void ipc_proc_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;

	// D0/D1 for UART8, arduino connector, on port J
	__HAL_RCC_GPIOJ_CLK_ENABLE();

	// Extra RCC for UART8
	RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_UART8;
	RCC_PeriphClkInit.Usart16ClockSelection = RCC_UART8CLKSOURCE_D2PCLK1;
	HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);

	__HAL_RCC_UART8_CLK_ENABLE();
	__HAL_RCC_DMA1_CLK_ENABLE();

	// Uart8
	GPIO_InitStruct.Pin       = GPIO_PIN_8|GPIO_PIN_9;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF8_UART8;
	HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);

	/* Configure the DMA handler for Transmission process */
	hdma_tx.Instance                 = USARTx_TX_DMA_STREAM;
	hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
	hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
	hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
	hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	hdma_tx.Init.Mode                = DMA_NORMAL;
	hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
	hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
	hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	hdma_tx.Init.MemBurst            = DMA_MBURST_INC4;
	hdma_tx.Init.PeriphBurst         = DMA_PBURST_INC4;
	hdma_tx.Init.Request             = USARTx_TX_DMA_REQUEST;
	HAL_DMA_Init(&hdma_tx);

	/* Associate the initialised DMA handle to the UART handle */
	__HAL_LINKDMA(&UART8_Handle, hdmatx, hdma_tx);

	/* Configure the DMA handler for reception process */
	hdma_rx.Instance                 = USARTx_RX_DMA_STREAM;
	hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
	hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
	hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
	hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	hdma_rx.Init.Mode                = DMA_NORMAL;
	hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;
	hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
	hdma_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	hdma_rx.Init.MemBurst            = DMA_MBURST_INC4;
	hdma_rx.Init.PeriphBurst         = DMA_PBURST_INC4;
	hdma_rx.Init.Request             = USARTx_RX_DMA_REQUEST;
	HAL_DMA_Init(&hdma_rx);

	/* Associate the initialised DMA handle to the the UART handle */
	__HAL_LINKDMA(&UART8_Handle, hdmarx, hdma_rx);

	UART8_Handle.Instance            = UART8;
	UART8_Handle.Init.BaudRate       = 115200;
	UART8_Handle.Init.WordLength     = UART_WORDLENGTH_8B;
	UART8_Handle.Init.StopBits       = UART_STOPBITS_1;
	UART8_Handle.Init.Parity         = UART_PARITY_NONE;
	UART8_Handle.Init.Mode           = UART_MODE_TX_RX;
	UART8_Handle.Init.HwFlowCtl      = UART_HWCONTROL_NONE;
	UART8_Handle.Init.OverSampling   = UART_OVERSAMPLING_16;
	HAL_UART_Init( &UART8_Handle );

	/* NVIC configuration for DMA transfer complete interrupt (USART6_TX) */
	HAL_NVIC_SetPriority(USARTx_DMA_TX_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(USARTx_DMA_TX_IRQn);

	/* NVIC configuration for DMA transfer complete interrupt (USART6_RX) */
	HAL_NVIC_SetPriority(USARTx_DMA_RX_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(USARTx_DMA_RX_IRQn);

	#ifdef IPC_USE_IDLE_LINE
	// Enable Idle Line + TX complete interrupt
	__HAL_UART_ENABLE_IT(&UART8_Handle, UART_IT_TC|UART_IT_IDLE);
	__HAL_DMA_ENABLE_IT (&hdma_rx, 		DMA_IT_TC);
	__HAL_DMA_ENABLE_IT (&hdma_tx, 		DMA_IT_TC);
	#endif

	// NVIC for USART, to catch the TX complete and Line Idle
	HAL_NVIC_SetPriority(USARTx_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ  (USARTx_IRQn);

	// Flush UART
	memset(TxBuffer, 0x00, 10);
	HAL_UART_Transmit_DMA(&UART8_Handle, (uint8_t*)TxBuffer, 10);

	// Flush cache
	SCB_CleanDCache_by_Addr((uint32_t *)TxBuffer, sizeof(TxBuffer));
	SCB_CleanDCache_by_Addr((uint32_t *)RxBuffer, sizeof(RxBuffer));
}

unsigned char chechSumXOR(unsigned char *data, unsigned int len)
{
	unsigned char xorCS = 0x00;

	for (int i = 0; i < len; i++)
	{
		unsigned char extract = *data++;
		xorCS ^= extract;
	}
	return xorCS;
}

//*----------------------------------------------------------------------------
//* Function Name       : esp32_uart_exchange
//* Object              :
//* Notes    			: send and receive
//* Notes    			:
//* Context    			: CONTEXT_ESP32_TASK
//*----------------------------------------------------------------------------
uchar esp32_uart_exchange(uchar cmd, uchar *payload, uchar p_size, uchar *buffer, uchar *ret_size, ulong timeout)
{
	ulong 	i;
	ushort 	expected, out_size;
	uchar 	chksum;

	printf("-------------------------\r\n");

	// Flush cache
	SCB_CleanDCache_by_Addr((uint32_t *)TxBuffer, sizeof(TxBuffer));

	// Clear buffers
	memset(RxBuffer, 0, sizeof(RxBuffer));
	memset(TxBuffer, 0, sizeof(TxBuffer));

	#ifndef IPC_USE_IDLE_LINE
	expected	= 10;	// header size
	switch(cmd)
	{
		case MENU_READ_ESP_32_VERSION:
			expected += 17;
			break;
		case MENU_WIFI_GET_DETAILS:
			expected += 62;
			break;
		case MENU_WIFI_CONNECT_TO_NETWORK:
			expected += 1;
			break;
		case MENU_WIFI_GET_NETWORK_RSSI:
			expected += 4;
			break;
		default:
			break;
	}
	expected++;			// Checksum
	#endif

	// Init command
	TxBuffer[0] = cmd;		// command
	//TxBuffer[1] = seq;		// sequency ID

	if(payload != NULL)
	{
		memcpy(TxBuffer + 1, payload, p_size);
		out_size = 1 + p_size;
	}
	else
		out_size = 8;

	// Prepare transfer
	UartReady 	= RESET;
	UartError 	= RESET;
	i 			= 0;

	// Send command
	if(HAL_UART_Transmit_DMA(&UART8_Handle, (uint8_t*)TxBuffer, out_size) != HAL_OK)
		return 1;

	// Wait complete
	while(UartReady == RESET)
	{
		if(UartError == SET)
			return 2;

		if(i == timeout)
			return 3;

		vTaskDelay(1);
		i++;
	}

	// Prepare transfer
	UartReady 		= RESET;
	UartError 		= RESET;
	UartLineIdle 	= RESET;
	i 				= 0;

	#ifdef IPC_USE_IDLE_LINE
	// Start read
	int h_err = HAL_UART_Receive_DMA(&UART8_Handle, (uint8_t *)RxBuffer, 32);//sizeof(RxBuffer));
	if(h_err != HAL_OK)
	{
		printf("hal err %d\r\n", h_err);
		return 4;
	}

	while(UartReady == RESET)	//UartLineIdle == RESET)
	{
		//if(UartLineIdle == SET)
		//	break;

		if(i == timeout)
		{
			printf("UartLineIdle %d\r\n", UartLineIdle);
			if(UartLineIdle == SET)
			{
				expected = 32 - __HAL_DMA_GET_COUNTER(&hdma_rx);
				printf("data_length %d\r\n", expected);

				//HAL_DMA_Abort(&hdma_rx);
				HAL_UART_DMAStop(&UART8_Handle);
				break;
			}
			return 6;
		}

		vTaskDelay(1);
		i++;
	}
	#endif

	#ifndef IPC_USE_IDLE_LINE
	// Start read
	if(HAL_UART_Receive_DMA(&UART8_Handle, (uint8_t *)RxBuffer, expected) != HAL_OK)
		return 4;

	// Wait complete
	while(UartReady == RESET)
	{
		if(UartError == SET)
			return 5;

		if(i == timeout)
			return 6;

		vTaskDelay(1);
		i++;
	}
	#endif

	// Invalidate Data Cache to get the updated content of the SRAM
	SCB_InvalidateDCache_by_Addr((uint32_t *)RxBuffer, sizeof(RxBuffer));

	// Calculate checksum
	chksum = chechSumXOR(RxBuffer, (expected - 1));
	//printf("chk sum %02x\r\n", chksum);

	if(chksum != RxBuffer[expected - 1])
		return 7;

	// Copy to caller task
	if((buffer != NULL) && (ret_size != NULL))
	{
		print_hex_array(RxBuffer, expected);
		memcpy(buffer,RxBuffer, expected);
		*ret_size = expected;
	}

	//seq++;
	return 0;
}

static void ipc_proc_poll_rssi(void)
{
	uchar rssi_buf[20];
	uchar ret_size = 0, err;

	static uchar rssi_read_skip = 0;

	rssi_read_skip++;
	if(rssi_read_skip < 40)
		return;

	rssi_read_skip = 0;

	err = esp32_uart_exchange(MENU_WIFI_GET_NETWORK_RSSI, NULL, 0, rssi_buf, &ret_size, 200);
	if(err == 0)
	{
		//int rssi = ((rssi_buf[10] << 24)|(rssi_buf[11] << 16)|(rssi_buf[12] << 8)|(rssi_buf[13]));
		//printf("rssi %d\r\n", rssi);
	}
	//else
		//printf("err %d\r\n", err);
}

static void icc_proc_check_msg(void)
{
	osEvent event;

	// Wait for a short time for pending messages
	event = osMessageGet(hEspMessage, 50);
	if(event.status != osEventMessage)
		return;

	// Check status type
	if(event.status != osEventMessage)
		return;

	struct ESPMessage *esp_msg = (struct ESPMessage *)event.value.p;

	//printf("[uart] msg id: %d \r\n", esp_msg->ucMessageID);
	switch(esp_msg->ucMessageID)
	{
		// Read firmware version
		case 1:
			esp_msg->ucExecResult = esp32_uart_exchange(MENU_READ_ESP_32_VERSION, NULL, 0, esp_msg->ucData,&esp_msg->ucDataReady, 200);
			break;

		// Read WiFi details
		case 2:
			esp_msg->ucExecResult = esp32_uart_exchange(MENU_WIFI_GET_DETAILS, NULL, 0, esp_msg->ucData,&esp_msg->ucDataReady, 200);
			break;

		// Connect to WiFi network
		case 3:
		{
			esp_msg->ucExecResult = esp32_uart_exchange(MENU_WIFI_CONNECT_TO_NETWORK, (uchar *)vc, sizeof(vc), esp_msg->ucData,&esp_msg->ucDataReady, 200);
			if(esp_msg->ucExecResult == 0)
			{
				// Reboot it
				esp32_uart_exchange(MENU_ESP32_REBOOT, NULL, 0, NULL, NULL, 200);

				// Give it time to reboot and connect to wifi network
				vTaskDelay(3000);
			}
			break;
		}

		default:
			printf("no handler for msg: %d\r\n",esp_msg->ucMessageID);
			goto complete;
	}
	printf("uart comm res: %d\r\n",esp_msg->ucExecResult);

complete:
	// Mark as complete
	esp_msg->ucProcStatus = TASK_PROC_DONE;
}

//*----------------------------------------------------------------------------
//* Function Name       : ipc_proc_task
//* Object              :
//* Notes    			:
//* Notes    			:
//* Context    			: CONTEXT_IPC_PROC
//*----------------------------------------------------------------------------
void ipc_proc_task(void const *arg)
{
	vTaskDelay(5000);
	printf("ipc task start\r\n");

	for(;;)
	{
		icc_proc_check_msg();
		ipc_proc_poll_rssi();
	}
}
#endif
