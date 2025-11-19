/*
 * app_uwb.h
 *
 *  Created on: Jul 24, 2025
 *      Author: Hoàng Quân
 */

#ifndef APP_UWB_APP_UWB_H_
#define APP_UWB_APP_UWB_H_
// === Includes ===
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "deca_device_api.h"
#include "deca_regs.h"
#include "deca_spi.h"
#include "port.h"
typedef enum {
	  SLOT_BEACON = 0,
	  SLOT_TWR0,
	  SLOT_TWR1,
	  SLOT_TWR2,
	  SLOT_TWR3,
	  SLOT_DIS_TX,
	  SLOT_ITEM_ORIENTATION,
	  SLOT_GUIDE,
} SlotType;

typedef struct{
  uint16_t C;
  uint16_t N;
} rssi_raw_data_t;

extern uint8_t is_synced;
extern SlotType prev_slot_index;
// === function ===
// TAG
void transmit_beacon(uint8_t slotNum);
uint8_t receive_beacon_and_sync(uint8_t time);
//uint8_t receive_beacon_and_sync(uint16_t timeout_ms);
void app_uwb_init(void);
void twr_tag(double *distance);
void twr_anchor(void);
void distance_send(double dis0, double dis1, double dis2, double dis3);
void item_orientation_send(void);
void guide_revc(void);
void resp_msg_set_ts(uint8 *ts_field, const uint64 ts);
void resp_msg_get_ts(uint8 *ts_field, uint32 *ts);
void dis_msg_set(uint8 *dis_field, const uint16 dis);
uint64 get_rx_timestamp_u64(void);
void app_uwb_process_beacon_tag(void);
void app_uwb_process_twr_tag(double *dis0, double *dis1, double *dis2, double *dis3);
void app_uwb_process_dist_send(double dis0, double dis1, double dis2, double dis3);
void app_uwb_process_item_orientation_send(void);
void app_uwb_process_guide_revc(void);

// Anchor
void distance_revc(uint16_t *dis0, uint16_t *dis1, uint16_t *dis2, uint16_t *dis3);
void item_orientation_revc(void);
void guide_send(void);
void dis_msg_get(uint8_t *dis_field, uint16_t *dis);
void app_uwb_process_beacon_anchor(uint8_t anchor_id);
void app_uwb_process_twr_anchor(uint8_t anchor_id);
void app_uwb_process_dist_revc(uint16_t *dis0, uint16_t *dis1, uint16_t *dis2, uint16_t *dis3, uint8_t anchor_id);
void app_uwb_process_item_orientation_revc(uint8_t anchor_id);
void app_uwb_process_guide_send(uint8_t anchor_id);
int32_t get_signal_quality(void);
void get_signal_data(rssi_raw_data_t *raw_data);
float get_rssi_dBm(const rssi_raw_data_t *raw_data, uint8_t prf);

#endif /* APP_UWB_APP_UWB_H_ */
