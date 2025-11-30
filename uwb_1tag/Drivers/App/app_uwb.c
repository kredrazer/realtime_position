/*
 * app_uwb.c
 *
 *  Created on: Jul 24, 2025
 *      Author: Hoàng Quân
 *      Side_author: Tuấn Nguyễn
 */

#ifndef APP_UWB_APP_UWB_C_
#define APP_UWB_APP_UWB_C_
#include "app_uwb.h"
#include "deca_device_api.h"
#include "deca_regs.h"
#include "deca_spi.h"
#include "port.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdint.h>
#include "app_uart_handler.h"
#include "app_uart.h"
#include <math.h>
#include "kalman_filter.h"
#include "trilateration.h"
typedef unsigned long long uint64;

#define TX_ANT_DLY 16436
#define RX_ANT_DLY 16436
#define ALL_MSG_COMMON_LEN 10
/* Index to access some of the fields in the frames involved in the process. */
#define ALL_MSG_SN_IDX 2
#define RESP_MSG_POLL_RX_TS_IDX 10
#define RESP_MSG_RESP_TX_TS_IDX 14
#define DIS0_IDX 10
#define DIS1_IDX 12
#define DIS2_IDX 14
#define DIS3_IDX 16
#define RESP_MSG_TS_LEN 4
/* UWB microsecond (uus) to device time unit (dtu, around 15.65 ps) conversion factor.
 * 1 uus = 512 / 499.2 �s and 1 �s = 499.2 * 128 dtu. */
#define UUS_TO_DWT_TIME 65536

/* Delay between frames, in UWB microseconds. See NOTE 1 below. */
#define POLL_RX_TO_RESP_TX_DLY_UUS 500
#define RX_BUF_LEN 20
#define UUS_TO_DWT_TIME 65536
#define POLL_TX_TO_RESP_RX_DLY_UUS 300
#define RESP_RX_TIMEOUT_UUS 400
#define SPEED_OF_LIGHT 299702547
#define CIR_PWR_OFFSET 0x06
#define DWT_TIME_UNITS          (1.0/499.2e6/128.0) //!< = 15.65e-12 
#define TICKS_PER_MS       (499.2e6*128/1000)
#define TX_DELAY_MS_coeff   (TICKS_PER_MS/256)
#define MS_TO_DELAY_UNIT(x)  ((uint32_t)((x)*TX_DELAY_MS_coeff))
#define SLOT_DURATION_MS        10
#define SUPERFRAME_SLOTS        10

static uint8_t missed_beacon_count = 0;
static uint64_t beacon_rx_ts_64 =0;
static unsigned next_beacon_tx_time_unit=0;
#define MAX_MISSED_BEACONS 3 
static uint8 beacon_msg[] =  {0x41, 0x88, 0, 0xCA, 0xDE, 'B', 'E', 'A', 'C', 0xE2, 0, 0, 0};
static uint8 tx_poll_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 'P', 'O', 'L', 'L', 0xE0, 0, 0};

static uint8 rx_poll_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 'P', 'O', 'L', 'L', 0xE0, 0, 0};
static uint8 tx_resp_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 'R', 'E', 'S', 'P', 0xE1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static uint8 rx_resp_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 'R', 'E', 'S', 'P', 0xE1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static uint8 dis_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 'D', 'I', 'S', 'T', 0xE3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static uint8 guide_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 'G', 'U', 'I', 'D', 0xE5, 0, 0, 0, 0};
static uint8 item_orientation_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 'I', 'T', 'E', 'M',  0xE6, 0, 0, 0, 0, 0};
static uint8 rx_buffer[RX_BUF_LEN];
static uint8 rx_buffer_new[RX_BUF_LEN];
static uint8 rx_buffer_guide[RX_BUF_LEN];
static uint8 rx_buffer_dis[25];
static uint8 rx_buffer_item[25];
AnchorStats_t anchor_database[4] = {
    {0, 0.0, 0.0, -999.0, 0.0},
    {1, 5.0, 0.0, -999.0, 0.0},
    {2, 0.0, 5.0, -999.0, 0.0},
    {3, 5.0, 5.0, -999.0, 0.0}
};

vec2d_t final_position = {0.0, 0.0};
extern vec2d_t anchor_pos[4];
extern kalmanFilter kf_x;
extern kalmanFilter kf_y;
char buff[50];

static dwt_config_t config = {
	  5,               /* Channel number. */
	  DWT_PRF_64M,     /* Pulse repetition frequency. */
	  DWT_PLEN_128,    /* Preamble length. Used in TX only. */
	  DWT_PAC8,        /* Preamble acquisition chunk size. Used in RX only. */
	  9,               /* TX preamble code. Used in TX only. */
	  9,               /* RX preamble code. Used in RX only. */
	  0,               /* 0 to use standard SFD, 1 to use non-standard SFD. */
	  DWT_BR_6M8,      /* Data rate. */
	  DWT_PHRMODE_STD, /* PHY header mode. */
	  (129 + 8 - 8)    /* SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
};
static dwt_txconfig_t txconfig = {
      0xC0,            // PG delay (giá trị chuẩn, không đổi)
      0x15355575       // TX power (recommended for 64M PRF, Ch 2 & 5)
  };

 uint8_t is_synced = 0;
 SlotType prev_slot_index = 99;
 volatile float rssi_dbm =0.0f;
 volatile int32_t chat_luong=0;

void app_uwb_init(void)
{
  reset_DW1000();
  port_set_dw1000_slowrate();
  if (dwt_initialise(DWT_LOADUCODE) == DWT_ERROR)
  {
	while (1)
	{ };
  }
  port_set_dw1000_fastrate();
  dwt_configure(&config);

  dwt_setrxantennadelay(RX_ANT_DLY);
  dwt_settxantennadelay(TX_ANT_DLY);
}

//Anchor
void transmit_beacon(uint8_t slotNum)
{
  dwt_forcetrxoff();
  uint32_t start_tick = HAL_GetTick();
  beacon_msg[10] = slotNum;
  dwt_writetxdata(sizeof(beacon_msg), beacon_msg, 0);
  dwt_writetxfctrl(sizeof(beacon_msg), 0, 0);
  dwt_starttx(DWT_START_TX_IMMEDIATE);
  start_tick = HAL_GetTick();
          while (!(dwt_read32bitreg(SYS_STATUS_ID) & SYS_STATUS_TXFRS))
          {
            if (HAL_GetTick() - start_tick > 7) {
          	  dwt_forcetrxoff();
              return;
            }
          }
  dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);
}
// void transmit_beacon(uint8_t slotNum)
//   {
//     beacon_msg[10]=slotNum;
//     dwt_writetxdata(sizeof(beacon_msg), beacon_msg, 0);
//     dwt_writetxfctrl(sizeof(beacon_msg), 0, 0);
//     uint32_t period_delay =  MS_TO_DELAY_UNIT(SUPERFRAME_SLOTS*SLOT_DURATION_MS);
//     int ret;
//     if(next_beacon_tx_time_unit==0)
//       {
//         dwt_forcetrxoff();
//         dwt_starttx(DWT_START_TX_IMMEDIATE);
//         while(!(dwt_read32bitreg(SYS_STATUS_ID)&SYS_STATUS_TXFRS)) {};
//             dwt_write32bitreg(SYS_STATUS_ID,SYS_STATUS_TXFRS);
//         uint32_t raw_tx_time = dwt_readtxtimestamplo32();
//         next_beacon_tx_time_unit = (raw_tx_time>>8)+period_delay;
//         return;
//       }
//     dwt_setdelayedtrxtime(next_beacon_tx_time_unit & 0xFFFFFFFE);
//     ret = dwt_starttx(DWT_START_TX_DELAYED);
//     if (ret == DWT_SUCCESS)
//       {
//         next_beacon_tx_time_unit += period_delay;
//         while (!(dwt_read32bitreg(SYS_STATUS_ID) & SYS_STATUS_TXFRS)) {};
//         dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);
//         HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);
//       }
//     else
//       {
//         dwt_starttx(DWT_START_TX_IMMEDIATE);
        
//         while (!(dwt_read32bitreg(SYS_STATUS_ID) & SYS_STATUS_TXFRS)) {};
//         dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);
//         uint32_t raw_tx_time = dwt_readtxtimestamplo32();
//         next_beacon_tx_time_unit = (raw_tx_time >> 8) + period_delay;
//       }
//   }
uint8_t receive_beacon_and_sync(uint8_t time)
{
  dwt_forcetrxoff();
  dwt_rxenable(DWT_START_RX_IMMEDIATE);
  memset(rx_buffer_new, 0, 20);
  uint32_t status_reg;
  uint32_t start_tick = HAL_GetTick();
  
  while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) &
            (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_ERR)))
  {
    if (HAL_GetTick() - start_tick > time)
    {
        dwt_forcetrxoff();
        return 0;  // Timeout - không nhận được beacon
    }
  }
  
  if (status_reg & SYS_STATUS_RXFCG)
  {
    uint16_t timeRevc = TIM1->CNT;
    uint32_t frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFL_MASK_1023;
    if (frame_len > 20) 
    {
      dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);
      return 0;
    }
    
    dwt_readrxdata(rx_buffer_new, frame_len, 0);
    
    if (memcmp(rx_buffer_new, beacon_msg, 10) == 0)
    {
      uint16_t beacon_slot = rx_buffer_new[10];
      uint16_t timeSync = TIM1->CNT;
      uint16_t delta = (timeSync + 1500 - timeRevc) % 1500;
      uint32_t slotStart = beacon_slot * 100;
      TIM1->CNT = slotStart + delta + 4;
      
      dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);
      return 1;  // Thành công
    }
    dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);
  }
  
  dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_ERR);
  dwt_rxreset();
  return 0;
}


//TAG
void twr_tag(double *distance,uint8_t anchor_idx)
{
  dwt_forcetrxoff();
  dwt_setrxaftertxdelay(POLL_TX_TO_RESP_RX_DLY_UUS);
  dwt_setrxtimeout(RESP_RX_TIMEOUT_UUS);
  tx_poll_msg[ALL_MSG_SN_IDX] = 0;
  dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);
  dwt_writetxdata(sizeof(tx_poll_msg), tx_poll_msg, 0);
  dwt_writetxfctrl(sizeof(tx_poll_msg), 0, 1);
  dwt_starttx(DWT_START_TX_IMMEDIATE | DWT_RESPONSE_EXPECTED);
  uint32_t start_tick = HAL_GetTick();
  uint32_t status_reg;
  do
  {
      status_reg = dwt_read32bitreg(SYS_STATUS_ID);
      if (HAL_GetTick() - start_tick > 2)
      {
        dwt_forcetrxoff();
        return;
      }
  }
  while (!(status_reg & (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR)));

  if (status_reg & SYS_STATUS_RXFCG)
  {
	uint32 poll_tx_ts, resp_rx_ts, poll_rx_ts, resp_tx_ts;
    uint32 frame_len;
    //ham tinh rssi theo 2 cach
    chat_luong = get_signal_quality();
    rssi_raw_data_t my_raw_rssi;
    get_signal_data(&my_raw_rssi);
    rssi_dbm = get_rssi_dBm(&my_raw_rssi, config.prf);
    //end
    dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);
    frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFLEN_MASK;
    if (frame_len <= RX_BUF_LEN)  dwt_readrxdata(rx_buffer, frame_len, 0);

    rx_resp_msg[ALL_MSG_SN_IDX] = 0;
    if (memcmp(rx_buffer, rx_resp_msg, ALL_MSG_COMMON_LEN) == 0)
    {
      float clockOffsetRatio;
      poll_tx_ts = dwt_readtxtimestamplo32();
      resp_rx_ts = dwt_readrxtimestamplo32();
      clockOffsetRatio = dwt_readcarrierintegrator() * (FREQ_OFFSET_MULTIPLIER * HERTZ_TO_PPM_MULTIPLIER_CHAN_2 / 1.0e6);

      resp_msg_get_ts(&rx_buffer[RESP_MSG_POLL_RX_TS_IDX], &poll_rx_ts);
      resp_msg_get_ts(&rx_buffer[RESP_MSG_RESP_TX_TS_IDX], &resp_tx_ts);

      int32 rtd_init = resp_rx_ts - poll_tx_ts;
      int32 rtd_resp = resp_tx_ts - poll_rx_ts;

      double tof = ((rtd_init - rtd_resp * (1 - clockOffsetRatio)) / 2.0) * DWT_TIME_UNITS;
      *distance = tof * SPEED_OF_LIGHT;
      anchor_database[anchor_idx].rssi= rssi_dbm;
      anchor_database[anchor_idx].dist =*distance;

    }
  }
  else
  {
    dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);
    dwt_rxreset();
     anchor_database[anchor_idx].rssi=-999.0f;
     anchor_database[anchor_idx].dist=0;
  }
}

//Anchor
void twr_anchor(void)
{
  dwt_forcetrxoff();
  dwt_rxenable(DWT_START_RX_IMMEDIATE);
  uint32_t start_tick = HAL_GetTick();
  uint32_t status_reg;
  do {
      status_reg = dwt_read32bitreg(SYS_STATUS_ID);
      if (HAL_GetTick() - start_tick > 7)
      {
    	  dwt_forcetrxoff();
          return;
      }
  } while (!(status_reg & (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_ERR)));

  if (status_reg & SYS_STATUS_RXFCG)
  {
    uint32 frame_len;
    uint64 poll_rx_ts;
    uint64 resp_tx_ts;
    dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);
    frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFL_MASK_1023;

    if (frame_len <= RX_BUFFER_LEN)
    {
      dwt_readrxdata(rx_buffer, frame_len, 0);
    }
    rx_poll_msg[ALL_MSG_SN_IDX] = 0;
    if (memcmp(rx_buffer, rx_poll_msg, ALL_MSG_COMMON_LEN) == 0)
    {
      poll_rx_ts = get_rx_timestamp_u64();
      uint64_t resp_tx_time = (poll_rx_ts + (POLL_RX_TO_RESP_TX_DLY_UUS * UUS_TO_DWT_TIME)) >> 8;
      dwt_setdelayedtrxtime(resp_tx_time);
      resp_tx_ts = (((uint64)(resp_tx_time & 0xFFFFFFFEUL)) << 8) + TX_ANT_DLY;
      resp_msg_set_ts(&tx_resp_msg[RESP_MSG_POLL_RX_TS_IDX], poll_rx_ts);
      resp_msg_set_ts(&tx_resp_msg[RESP_MSG_RESP_TX_TS_IDX], resp_tx_ts);

      tx_resp_msg[ALL_MSG_SN_IDX] = 0;
      dwt_writetxdata(sizeof(tx_resp_msg), tx_resp_msg, 0);
      dwt_writetxfctrl(sizeof(tx_resp_msg), 0, 1);
      int ret = dwt_starttx(DWT_START_TX_DELAYED);

      if (ret == DWT_SUCCESS)
      {
        start_tick = HAL_GetTick();
        while (!(dwt_read32bitreg(SYS_STATUS_ID) & SYS_STATUS_TXFRS))
        {
          if (HAL_GetTick() - start_tick > 1) {
        	  dwt_forcetrxoff();
            return;
          }
        }
        dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);
      }
    }
  }
  else
  {
    dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_ERR);
    dwt_rxreset();
  }
}

//TAG
void distance_send(double dis0, double dis1, double dis2, double dis3)
{
	dwt_forcetrxoff();
	uint32_t start_tick = HAL_GetTick();
	dis_msg_set(&dis_msg[DIS0_IDX], (uint16_t)(dis0*100));
	dis_msg_set(&dis_msg[DIS1_IDX], (uint16_t)(dis1*100));
	dis_msg_set(&dis_msg[DIS2_IDX], (uint16_t)(dis2*100));
	dis_msg_set(&dis_msg[DIS3_IDX], (uint16_t)(dis3*100));
	dwt_writetxdata(sizeof(dis_msg), dis_msg, 0);
	dwt_writetxfctrl(sizeof(dis_msg), 0, 0);
	dwt_starttx(DWT_START_TX_IMMEDIATE);
	start_tick = HAL_GetTick();

	while (!(dwt_read32bitreg(SYS_STATUS_ID) & SYS_STATUS_TXFRS))
	{
	  if (HAL_GetTick() - start_tick > 3) {
		  dwt_forcetrxoff();
		return;
	  }
	}
	dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);
}

//Anchor
void distance_revc(uint16_t *dis0, uint16_t *dis1, uint16_t *dis2, uint16_t *dis3)
{
  dwt_forcetrxoff();
  dwt_rxenable(DWT_START_RX_IMMEDIATE);
  uint32_t start_tick = HAL_GetTick();
  uint32_t status_reg;
  do {
      status_reg = dwt_read32bitreg(SYS_STATUS_ID);
      if (HAL_GetTick() - start_tick > 7) {
          dwt_forcetrxoff();
          return;
      }
  } while (!(status_reg & (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_ERR)));
  if (status_reg & SYS_STATUS_RXFCG)
  {
    uint32 frame_len;
    frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFL_MASK_1023;
    if (frame_len <= RX_BUFFER_LEN)
    {
      dwt_readrxdata(rx_buffer_dis, frame_len, 0);
    }
    if (memcmp(rx_buffer_dis, dis_msg, ALL_MSG_COMMON_LEN) == 0)
    {
      dis_msg_get(&rx_buffer_dis[DIS0_IDX], dis0);
      dis_msg_get(&rx_buffer_dis[DIS1_IDX], dis1);
      dis_msg_get(&rx_buffer_dis[DIS2_IDX], dis2);
      dis_msg_get(&rx_buffer_dis[DIS3_IDX], dis3);
    }
    dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);
  }
  dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_ERR);
  dwt_rxreset();
}

//TAG
void item_orientation_send(void)
{
	dwt_forcetrxoff();
	uint32_t start_tick = HAL_GetTick();
	item_orientation_msg[10] = rx_buf[1];
	item_orientation_msg[11] = rx_buf[2];
	item_orientation_msg[12] = rx_buf[3];
	dwt_writetxdata(sizeof(item_orientation_msg), item_orientation_msg, 0);
	dwt_writetxfctrl(sizeof(item_orientation_msg), 0, 0);
	dwt_starttx(DWT_START_TX_IMMEDIATE);
	start_tick = HAL_GetTick();
	while (!(dwt_read32bitreg(SYS_STATUS_ID) & SYS_STATUS_TXFRS))
	{
	  if (HAL_GetTick() - start_tick > 3)
	  {
		  dwt_forcetrxoff();
		  return;
	  }
	}
	dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);
}

//Anchor
void item_orientation_revc(void)
{
  dwt_forcetrxoff();
  memset(rx_buffer_item, 0, 25);
  dwt_rxenable(DWT_START_RX_IMMEDIATE);
  uint32_t start_tick = HAL_GetTick();
  uint32_t status_reg;
  do 
  {
      status_reg = dwt_read32bitreg(SYS_STATUS_ID);
      if (HAL_GetTick() - start_tick > 7) 
      {
          dwt_forcetrxoff();
          return;
      }
  }
  while (!(status_reg & (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_ERR)));
  if (status_reg & SYS_STATUS_RXFCG)
  {

    uint32 frame_len;
    frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFL_MASK_1023;
    if (frame_len <= RX_BUFFER_LEN)
    {
      dwt_readrxdata(rx_buffer_item, frame_len, 0);
    }
    if (memcmp(rx_buffer_item, item_orientation_msg, ALL_MSG_COMMON_LEN) == 0)
    {
    }
    dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);
  }
  dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_ERR);
  dwt_rxreset();
}

//TAG
void guide_revc(void)
{
	dwt_forcetrxoff();
	uint32 status_reg = 0;
	dwt_rxenable(DWT_START_RX_IMMEDIATE);
	uint32_t start_tick = HAL_GetTick();
	while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) & (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_ERR)))
    {
	  if (HAL_GetTick() - start_tick > 7)
	  {
		dwt_forcetrxoff();
		return;
	  }
    };

	if (status_reg & SYS_STATUS_RXFCG)
	{
		uint32 frame_len;
		frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFL_MASK_1023;
		if (frame_len <= RX_BUFFER_LEN)
		{

		  dwt_readrxdata(rx_buffer_guide, frame_len, 0);
		}

		if (memcmp(rx_buffer_guide, guide_msg, ALL_MSG_COMMON_LEN) == 0)
		{

		}
		dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);
	}
	else
	{
		dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_ERR);
		dwt_rxreset();
	}
}

//Anchor
void guide_send(void)
{
    dwt_forcetrxoff();
    guide_msg[10] = rx_buf[6];
    guide_msg[11] = rx_buf[9];
    guide_msg[12] = rx_buf[10];
    dwt_writetxdata(sizeof(guide_msg), guide_msg, 0);
    dwt_writetxfctrl(sizeof(guide_msg), 0, 0);
    dwt_starttx(DWT_START_TX_IMMEDIATE);
    uint32_t start_tick = HAL_GetTick();
    while (!(dwt_read32bitreg(SYS_STATUS_ID) & SYS_STATUS_TXFRS))
    {
      if (HAL_GetTick() - start_tick > 6) 
      {
        dwt_forcetrxoff();
        return;
      }
    }
    dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);
}

uint64 get_rx_timestamp_u64(void)
{
  uint8 ts_tab[5];
  uint64 ts = 0;
  int i;
  dwt_readrxtimestamp(ts_tab);
  for (i = 4; i >= 0; i--)
  {
    ts <<= 8;
    ts |= ts_tab[i];
  }
  return ts;
}

void resp_msg_set_ts(uint8 *ts_field, const uint64 ts)
{
  int i;
  for (i = 0; i < RESP_MSG_TS_LEN; i++)
	  ts_field[i] = (ts >> (i * 8)) & 0xFF;
}

void resp_msg_get_ts(uint8 *ts_field, uint32 *ts)
{
  int i;
  *ts = 0;
  for (i = 0; i < RESP_MSG_TS_LEN; i++)
  {
      *ts += ts_field[i] << (i * 8);
  }
}

//TAG
void dis_msg_set(uint8_t *dis_field, const uint16_t dis)
{
  for (int i = 0; i < 2; i++)  // vì uint16_t là 2 byte
  {
    dis_field[i] = (dis >> (i * 8)) & 0xFF;
  }
}

//Anchor
void dis_msg_get(uint8_t *dis_field, uint16_t *dis)
{
  *dis = 0;
  for (int i = 0; i < 2; i++)  // vì uint16_t là 2 byte
  {
    *dis |= ((uint16_t)dis_field[i]) << (i * 8);
  }
}

void app_uwb_process_beacon_tag(void)
{
	// Nếu chưa đồng bộ, ưu tiên hàng đầu là tìm lại beacon
	if (!is_synced)
	{
		// Bật bộ thu với timeout dài để chắc chắn bắt được beacon tiếp theo
		if (receive_beacon_and_sync(157)) // 157ms > 1 superframe (100ms)
		{
			is_synced = 1;
			missed_beacon_count = 0;
			// Không cần set prev_slot_index ở đây, vì timer đã được sync
		}
		return; // Kết thúc, chờ vòng lặp tiếp theo
	}

	// Nếu đã đồng bộ, chỉ xử lý khi vào đúng slot
	uint8_t slot_index = (TIM1->CNT) / 100;
	if (slot_index == SLOT_BEACON && slot_index != prev_slot_index)
	{
		prev_slot_index = slot_index;

		// Lắng nghe beacon với timeout ngắn để xác nhận lại đồng bộ
		if (receive_beacon_and_sync(15))
		{
			missed_beacon_count = 0; // Đồng bộ tốt, reset bộ đếm
		}
		else
		{
			// Bỏ lỡ beacon
			missed_beacon_count++;
			if (missed_beacon_count >= MAX_MISSED_BEACONS)
			{
				is_synced = 0; // Mất đồng bộ
			}
		}
	}
}

// ANCHOR - Sửa lại logic tương tự
void app_uwb_process_beacon_anchor(uint8_t anchor_id) 
{
  if (anchor_id == 0)
  {
    // ANCHOR CHÍNH: Logic không đổi, chỉ phát beacon ở đúng slot
    uint8_t slot_index = (TIM1->CNT) / 100;
    if (slot_index != prev_slot_index && slot_index == SLOT_BEACON)
    {
      prev_slot_index = slot_index;
      is_synced = 1;
      missed_beacon_count = 0;
      transmit_beacon(slot_index);
    }
  }
  else
  {
    // ANCHOR PHỤ: Logic đồng bộ được ưu tiên hàng đầu
    // 1. Nếu chưa đồng bộ, hãy tìm kiếm beacon ngay lập tức
    if (!is_synced)
    {
      // Bật bộ thu với timeout dài để chắc chắn bắt được beacon tiếp theo
      if (receive_beacon_and_sync(157)) // 157ms > 1 superframe (100ms)
      {
        is_synced = 1;
        missed_beacon_count = 0;
      }
      // Dù thành công hay không, cũng không làm gì khác cho đến khi đồng bộ được
      return;
    }

    // 2. Nếu đã đồng bộ, hoạt động theo slot
    uint8_t slot_index = (TIM1->CNT) / 100;
    if (slot_index != prev_slot_index && slot_index == SLOT_BEACON)
    {
      prev_slot_index = slot_index;
      // Lắng nghe beacon với timeout ngắn để xác nhận lại đồng bộ
      if (receive_beacon_and_sync(15))
      {
        missed_beacon_count = 0; // Đồng bộ tốt, reset bộ đếm
      }
      else
      {
        // Bỏ lỡ beacon
        missed_beacon_count++;
        if (missed_beacon_count >= MAX_MISSED_BEACONS)
        {
          is_synced = 0; // Mất đồng bộ, vòng lặp sau sẽ vào trạng thái tìm kiếm
        }
      }
    }
  }
  // Logic xử lý cho các slot khác chỉ nên chạy khi is_synced = 1
}

// Thêm hàm helper để check trạng thái sync (optional)
uint8_t is_system_synchronized(void)
{
  return is_synced;
}

uint8_t get_missed_beacon_count(void)
{
  return missed_beacon_count;
}
//TAG
void app_uwb_process_twr_tag(double *dis0, double *dis1, double *dis2, double *dis3)
{
	uint8_t slot_index = (TIM1->CNT) / 100;
	if (is_synced && slot_index != prev_slot_index)
	{
      prev_slot_index = slot_index;
      switch(slot_index)
      {
          case SLOT_TWR0:
              twr_tag(dis0,0);
              break;
          case SLOT_TWR1:
              twr_tag(dis1,1);
              break;
          case SLOT_TWR2:
              twr_tag(dis2,2);
              break;
          case SLOT_TWR3:
              twr_tag(dis3,3);
              break;
      }
	}
}

//Anchor
void app_uwb_process_twr_anchor(uint8_t anchor_id) 
{
  uint8_t slot_index = (TIM1->CNT) / 100;
  if (slot_index != prev_slot_index) 
  {
    if (anchor_id == 0) 
    {
        if (slot_index == SLOT_TWR0) 
        {
          prev_slot_index = slot_index;
          twr_anchor();
        }
    } 
    else 
    {
        if (is_synced && slot_index == anchor_id + 1) 
        {
          prev_slot_index = slot_index;
          twr_anchor();
        }
    }
  }
}

//TAG
void app_uwb_process_dist_send(double dis0, double dis1, double dis2, double dis3)
{
	uint8_t slot_index = (TIM1->CNT) / 100;
	if (is_synced && slot_index != prev_slot_index && slot_index == SLOT_DIS_TX)
	{
		prev_slot_index = slot_index;
    int worst_idx=0;
    float min_rssi=anchor_database[0].rssi;
    for(int i =1;i<4;i++)
    {
      if(anchor_database[i].rssi<min_rssi)
      {
        worst_idx=i;
        min_rssi=anchor_database[i].rssi;

      }
    }
    double d_send[4] = {dis0, dis1, dis2, dis3};
    d_send[worst_idx] = 0.0;
	distance_send(d_send[0], d_send[1], d_send[2], d_send[3]);
	}
}

//Anchor
void app_uwb_process_dist_revc(uint16_t *dis0, uint16_t *dis1, uint16_t *dis2, uint16_t *dis3, uint8_t anchor_id)
{
  uint8_t slot_index = (TIM1->CNT) / 100;
  if (is_synced && slot_index != prev_slot_index && slot_index == SLOT_DIS_TX) 
  {
      prev_slot_index = slot_index;
      if(anchor_id == 0)
      {
          distance_revc(dis0, dis1, dis2, dis3);
          // app_uart_dist(&huart1, rx_buffer_dis);
          // add_trilateration và lọc rồi nha
          float d[4];
          d[0]=(*dis0)/100.0f;
          d[1]=(*dis1)/100.0f;
          d[2]=(*dis2)/100.0f;
          d[3]=(*dis3)/100.0f;
          int valid_idx[4]; // Tăng kích thước để khớp với 4 anchor
          int cnt =0;
          for(int i =0; i<4;i++)
          {
            if(d[i] > 0.01f && cnt < 4) // Đảm bảo không ghi tràn và chỉ lấy tối đa 4 anchor hợp lệ
            {
              valid_idx[cnt++] = i;
            }
          }
          if(cnt >= 3) 
          {
            vec2d_t p1 = anchor_pos[valid_idx[0]]; float r1 = d[valid_idx[0]];
            vec2d_t p2 = anchor_pos[valid_idx[1]]; float r2 = d[valid_idx[1]];
            vec2d_t p3 = anchor_pos[valid_idx[2]]; float r3 = d[valid_idx[2]];
            vec2d_t raw_pos;
            if(trilateration_2d(p1, r1, p2, r2, p3, r3, &raw_pos)) // Sử dụng 3 anchor đầu tiên tìm được
            {
              // sử dụng kalman ở đây anh nhé :))
              float final_x = updateEstimate(&kf_x,raw_pos.x);
              float final_y = updateEstimate(&kf_y,raw_pos.y);
              // sprintf(buff, "RAW:%.2f,%.2f | KF:%.2f,%.2f\r\n", raw_pos.x, raw_pos.y, final_x, final_y);
              // HAL_UART_Transmit(&huart1, (uint8_t*)buff, strlen(buff), 10);
              app_uart_position(&huart1, final_x, final_y, anchor_id);
              // sprintf(buff, "RAW:%.2f,%.2f | KF:%.2f,%.2f\r\n", raw_pos.x, raw_pos.y, final_x, final_y);
              // HAL_UART_Transmit(&huart1, (uint8_t*)buff, strlen(buff), 10);
            }
          }
//          else if (cnt == 2) // Trường hợp chỉ có 2 anchor, chỉ in ra khoảng cách để test
//          {
//              float r1 = d[valid_idx[0]];
//              float r2 = d[valid_idx[1]];
//              sprintf(buff, "TEST_2_ANCHORS: D%d=%.2f, D%d=%.2f\r\n", valid_idx[0], r1, valid_idx[1], r2);
//              HAL_UART_Transmit(&huart1, (uint8_t*)buff, strlen(buff), 10);
//          }
    }
  }
}

//TAG
void app_uwb_process_item_orientation_send(void)
{
	uint8_t slot_index = (TIM1->CNT) / 100;
	if (is_synced && slot_index != prev_slot_index && slot_index == SLOT_ITEM_ORIENTATION)
	{
		prev_slot_index = slot_index;
		item_orientation_send();
		memset(rx_buf, 0, 14);
	}
}

//Anchor
void app_uwb_process_item_orientation_revc(uint8_t anchor_id)
{
  uint8_t slot_index = (TIM1->CNT) / 100;
  if (is_synced && slot_index != prev_slot_index && slot_index == SLOT_ITEM_ORIENTATION) {
      prev_slot_index = slot_index;
      if(anchor_id == 0)
      {

    	  item_orientation_revc();
    	  app_uart_item_orientation(&huart1, rx_buffer_item);
    	  rx_buffer_item[10] = 0x00;
      }
  }
}

//TAG
void app_uwb_process_guide_revc(void)
{
	uint8_t slot_index = (TIM1->CNT) / 100;
	if (is_synced && slot_index != prev_slot_index && slot_index == SLOT_GUIDE)
	{
		prev_slot_index = slot_index;
		guide_revc();
		app_uart_guide(&huart1, rx_buffer_guide);
	//      memset(rx_buffer_guide, 0, 20);
	}
}

//Anchor
void app_uwb_process_guide_send(uint8_t anchor_id)
{
  uint8_t slot_index = (TIM1->CNT) / 100;
  if (is_synced && slot_index != prev_slot_index && slot_index == SLOT_GUIDE) 
  {
      prev_slot_index = slot_index;
      if(anchor_id == 0) guide_send();
      memset(rx_buf,0,14);
  }
}
 int32_t get_signal_quality (void){
	dwt_rxdiag_t diagnostics;
	dwt_readdiagnostics(&diagnostics);
  int32_t quality = (int32_t)diagnostics.firstPathAmp2 - (int32_t)diagnostics.stdNoise;
  return quality;
 }
 void get_signal_data(rssi_raw_data_t *raw_data){
  raw_data->C = dwt_read16bitoffsetreg(RX_FQUAL_ID,CIR_PWR_OFFSET);
  uint32_t rx_finfo_reg;
  rx_finfo_reg = dwt_read32bitreg(RX_FINFO_ID);
  raw_data->N = (uint16_t)((rx_finfo_reg & RX_FINFO_RXPACC_MASK)>> RX_FINFO_RXPACC_SHIFT);
 }

static float calculate_rssi_formula(float base, float N, uint8_t prf)
{
    float A, corrFac;
    float rssi_dbm;
    if (prf == DWT_PRF_16M)
    {
        A = 113.77f;
        corrFac = 2.3334f;
    }
    else
    {
        A = 121.74f;
        corrFac = 1.1667f;
    }
    rssi_dbm = 10.0f * log10f(base / (N * N)) - A;
    if (rssi_dbm > -88.0f)
    {
        rssi_dbm += (rssi_dbm + 88.0f) * corrFac;
    }
    return rssi_dbm;
}
float get_rssi_dBm(const rssi_raw_data_t *raw_data, uint8_t prf)
{
    float C_float = (float)raw_data->C;
    float N_float = (float)raw_data->N;
    if (N_float == 0)
    {
        return -150.0f; 
    }

    float twoPower17 = 131072.0f;
    float base = C_float * twoPower17;

    return calculate_rssi_formula(base, N_float, prf);
}


#endif /* APP_UWB_APP_UWB_C_ */
