/*
 * app_uart_handler.c
 *
 *  Created on: Aug 25, 2025
 *      Author: Hoàng Quân
 */


#include "app_uart_handler.h"
#include "usart.h"
uint8_t rx_buf[14];
static uint8_t rx_byte;
static uint16_t rx_index = 0;

// TAG
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		if (rx_index < 13) rx_buf[rx_index++] = rx_byte;
		else rx_index = 0; // Nếu quá giới hạn -> reset lại

		// Nếu gặp byte kết thúc 0x0F
		if (rx_byte == 0x0F)
		{
			// Ở đây rx_buf[0..rx_index-1] chính là frame nhận được
			// Reset index để chuẩn bị frame mới
			rx_index = 0;
		}

		// Đăng ký nhận tiếp byte mới
		HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
	}
}

void UART_HE_Receive_IT_Tag(void)
{
	HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
}

// Anchor
void UART_HE_Receive_IT_Anchor(uint8_t anchor_id)
{
    if(anchor_id == 0) HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
}

uint8_t check_frame(uint8_t *frame, uint16_t len)
{
	if(len < 13) return 0;
	if(frame[0] != 0xF0) return 0;
	if(frame[len - 1] != 0x0F) return 0;
	if(frame[1] != 0x05) return 0;
	if(frame[2] != (len - 7)) return 0;
	if(frame[len - 3] != 0xFF) return 0;
	if(frame[3]  != 0x40 || frame[6] != 0x41) return 0;
	if(frame[4] != 0x01 || frame[7] != 0x02) return 0;
	uint8_t checksum = 0;
	for (int i = 0; i <= len - 2; i ++)
		checksum ^= frame[i];
	
	if (checksum != frame[len - 2]) return 0;
	return 1;
}