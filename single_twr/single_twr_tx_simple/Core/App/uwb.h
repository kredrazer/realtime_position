#ifndef __UWB_H__
#define __UWB_H__

#include <stdint.h>

// ---- Định nghĩa hằng số ----
#define SPEED_OF_LIGHT 299702547.0

#define POLL_TX_TO_RESP_RX_DLY_UUS 140
#define RESP_RX_TIMEOUT_UUS        210

#define TX_ANT_DLY 16436
#define RX_ANT_DLY 16436

#define RESP_MSG_TS_LEN 4
#define RX_BUF_LEN 20

// ---- Cấu trúc lưu timestamp ----
typedef struct {
    uint32_t poll_tx_ts;
    uint32_t resp_rx_ts;
    uint32_t poll_rx_ts;
    uint32_t resp_tx_ts;
} twr_timestamps_t;

// ---- Biến frame ----
extern uint8_t tx_poll_msg[];
extern uint8_t rx_resp_msg[];

// ---- Prototype các hàm ----
void UWB_Sendmessage(uint8_t *message, uint8_t length);
uint32_t send_poll_get_tx_timestamp(uint8_t *msg, uint8_t len);
int receive_response(twr_timestamps_t *ts);
double calculate_tof(twr_timestamps_t *ts);
double calculate_distance(double tof);
void resp_msg_get_ts(uint8_t *ts_field, uint32_t *ts);

#endif
