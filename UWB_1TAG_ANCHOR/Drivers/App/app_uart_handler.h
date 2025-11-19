/*
 * app_uart_handler.h
 *
 *  Created on: Aug 25, 2025
 *      Author: Hoàng Quân
 */

#ifndef APP_UART_APP_UART_HANDLER_H_
#define APP_UART_APP_UART_HANDLER_H_

#include "stm32f4xx_hal.h"

#define RX_BUF_SIZE   64

extern uint8_t rx_buf[14];

void UART_HE_Receive_IT_Tag(void);
void UART_HE_Receive_IT_Anchor(uint8_t anchor_id);
uint8_t check_frame(uint8_t *frame, uint16_t len);

#endif /* APP_UART_APP_UART_HANDLER_H_ */
