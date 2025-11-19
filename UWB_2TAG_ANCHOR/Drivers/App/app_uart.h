/*
 * app_uart.h
 *
 *  Created on: Aug 25, 2025
 *      Author: Hoàng Quân
 */

#ifndef APP_UART_APP_UART_H_
#define APP_UART_APP_UART_H_

#include <stdint.h>
#include "usart.h"

// TAG
void app_uart_guide(UART_HandleTypeDef *huart, uint8_t *buffer);
// Anchor
void app_uart_dist(UART_HandleTypeDef *huart, uint8_t *buffer,uint8_t *tag_id);
void app_uart_item_orientation(UART_HandleTypeDef *huart, uint8_t *buffer);

#endif /* APP_UART_APP_UART_H_ */
