/*
 * app_uart.c
 *
 *  Created on: Aug 25, 2025
 *      Author: Hoàng Quân
 */

#include "app_uart.h"
#include "usart.h"
#include "gpio.h"
#include <stdio.h>
#include <string.h>
#include "stm32f4xx_hal.h"

// TAG
typedef struct __attribute__((packed))
{
    uint8_t head;
    uint8_t direction;
    uint8_t distance[2];
    uint8_t tail;
} UART_Message_Guide;

void app_uart_guide(UART_HandleTypeDef *huart, uint8_t *buffer)
{
	UART_Message_Guide msg;
	if (buffer[10] == 0) return;
	msg.head = 0xF0;
	msg.tail = 0x0F;
	msg.direction = buffer[10];
	memcpy(msg.distance, &buffer[11], 2);
	HAL_UART_Transmit(huart, (uint8_t *)&msg, sizeof(UART_Message_Guide), 1);
}

// Anchor
typedef struct 
{
	uint8_t param_type;
	uint8_t param_len;
	uint8_t param_dis[2];
} sub_msg;

typedef struct __attribute__((packed)) 
{
    uint8_t head;
    uint8_t packet_type_1;
    uint8_t packet_type_2;
    uint8_t packet_len;
	sub_msg dis1;
	sub_msg dis2;
	sub_msg dis3;
	sub_msg dis4;
	uint8_t tag_id;
    uint8_t end_code;
    uint8_t check_code;
    uint8_t tail;
} UART_Message_Distance;

typedef struct __attribute__((packed)) 
{
    uint8_t head;
    uint8_t packet_type;
    uint8_t packet_len;
	uint8_t yaw_type;
	uint8_t yaw_len;
	uint8_t yaw[2];
	uint8_t item_type;
	uint8_t item_len;
	uint8_t item;
    uint8_t end_code;
    uint8_t check_code;
    uint8_t tail;
} UART_Message_Item_Orientation;

void app_uart_dist(UART_HandleTypeDef *huart, uint8_t *buffer,uint8_t *tag_id)
{
	if (buffer[10] == 0) return;
    
	UART_Message_Distance msg;
	msg.head = 0xF0;
	msg.packet_type_1 = 0x05;
	msg.packet_type_2 = 0x01;
	msg.packet_len = sizeof(UART_Message_Distance) - sizeof(msg.head) - sizeof(msg.packet_type_1) - sizeof(msg.packet_type_2)
	- sizeof(msg.packet_len) - sizeof(msg.check_code) - sizeof(msg.end_code) - sizeof(msg.tail);
	msg.end_code = 0xFF;
	msg.dis1.param_len = 2;
	msg.dis1.param_type = 103;
	memcpy(msg.dis1.param_dis, &buffer[10], 2);
	msg.dis2.param_len = 2;
	msg.dis2.param_type = 106;
	memcpy(msg.dis2.param_dis, &buffer[12], 2);
	msg.dis3.param_len = 2;
	msg.dis3.param_type = 109;
	memcpy(msg.dis3.param_dis, &buffer[14], 2);
	msg.dis4.param_len = 2;
	msg.dis4.param_type = 112;
	memcpy(msg.dis4.param_dis, &buffer[16], 2);
	msg.tag_id = *tag_id;
	uint8_t *raw = (uint8_t *)&msg;
	msg.check_code = 0;
	for (int i = 0; i < offsetof(UART_Message_Distance, check_code); i++) 
	{
	        msg.check_code ^= raw[i];
	}
	msg.tail = 0x0F;
	HAL_UART_Transmit(huart, (uint8_t *)&msg, sizeof(UART_Message_Distance), 1);
}

void app_uart_item_orientation(UART_HandleTypeDef *huart, uint8_t *buffer)
{
	UART_Message_Item_Orientation msg;
	if (buffer[10] == 0) return;
		    
	msg.head = 0xF0;
	msg.tail = 0x0F;
	msg.packet_type = 0x04;
	msg.packet_len = 2;
	msg.yaw_type = 0x32;
	msg.yaw_len = 2;
	msg.item_type = 0x20;
	msg.item_len = 1;
	msg.end_code = 0xFF;
	memcpy(msg.yaw, &buffer[11], 2);
	msg.item =  buffer[10];
	uint8_t *raw = (uint8_t *)&msg;
		msg.check_code = 0;
	for (int i = 0; i < offsetof(UART_Message_Item_Orientation, check_code); i++) 
	{
        msg.check_code ^= raw[i];
	}
	HAL_UART_Transmit(huart, (uint8_t *)&msg, sizeof(UART_Message_Item_Orientation), 1);
}
