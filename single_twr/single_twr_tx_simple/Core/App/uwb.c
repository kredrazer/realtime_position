#include "uwb.h"
#include "deca_device_api.h"
#include "deca_regs.h"
#include <string.h>
#include <stdio.h>

uint8_t tx_poll_msg[11] = {0x41,0x88,0,0xCA,0xDE,'W','A','V','E',0xE0,0,0};
uint8_t rx_resp_msg[] = {0x41,0x88,0,0xCA,0xDE,'V','E','W','A',0xE1,0,0,0,0,0,0,0,0,0,0};
static uint8_t rx_buffer[RX_BUF_LEN];

//---------------------------------------------
void UWB_Sendmessage(uint8_t *message, uint8_t length)
{
    dwt_writetxdata(length + 2, message, 0);
    dwt_writetxfctrl(length + 2, 0, 0);
    dwt_starttx(DWT_START_TX_IMMEDIATE);
    while (!(dwt_read32bitreg(SYS_STATUS_ID) & SYS_STATUS_TXFRS));
    dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);
    deca_sleep(100);
}

//---------------------------------------------
uint32_t send_poll_get_tx_timestamp(uint8_t *msg, uint8_t len)
{
    dwt_writetxdata(len, msg, 0);
    dwt_writetxfctrl(len, 0, 1);
    dwt_starttx(DWT_START_TX_IMMEDIATE | DWT_RESPONSE_EXPECTED);
    while(!(dwt_read32bitreg(SYS_STATUS_ID) & SYS_STATUS_TXFRS));
    dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);
    return dwt_readtxtimestamplo32();
}

//---------------------------------------------
void resp_msg_get_ts(uint8_t *ts_field, uint32_t *ts)
{
    *ts = ((uint32_t)ts_field[0]) |
          ((uint32_t)ts_field[1] << 8) |
          ((uint32_t)ts_field[2] << 16) |
          ((uint32_t)ts_field[3] << 24);
}

//---------------------------------------------
int receive_response(twr_timestamps_t *ts)
{
    uint32_t status_reg;
    dwt_setrxtimeout(10000); // ~10ms timeout
    dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS | SYS_STATUS_ALL_RX_ERR | SYS_STATUS_ALL_RX_TO);

    // KHÔNG cần forcetrxoff ở đây
    dwt_forcetrxoff();
    dwt_rxenable(DWT_START_RX_IMMEDIATE);

    uint32_t start = HAL_GetTick();
    while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) &
             (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR)))
    {
        if (HAL_GetTick() - start > 20) // 20ms safety timeout
            return 0;
    }

    if (status_reg & SYS_STATUS_RXFCG)
    {
        uint32_t frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFL_MASK_1023;
        if (frame_len <= RX_BUF_LEN)
            dwt_readrxdata(rx_buffer, frame_len, 0);

        dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);
        rx_buffer[2] = 0;

        if (memcmp(rx_buffer, rx_resp_msg, 9) == 0)
        {
            ts->resp_rx_ts = dwt_readrxtimestamplo32();
            resp_msg_get_ts(&rx_buffer[10], &ts->poll_rx_ts);
            resp_msg_get_ts(&rx_buffer[14], &ts->resp_tx_ts);
            return 1;
        }
    }
    else
    {
        dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);
        dwt_rxreset();
    }

    return 0;
}


//---------------------------------------------
double calculate_tof(twr_timestamps_t *ts)
{
    int32_t rtd_init = ts->resp_rx_ts - ts->poll_tx_ts;
    int32_t rtd_resp = ts->resp_tx_ts - ts->poll_rx_ts;
    double tof = (rtd_init - rtd_resp) / 2.0;
    tof *= DWT_TIME_UNITS;
    return tof;
}

double calculate_distance(double tof)
{
    return tof * SPEED_OF_LIGHT;
}

