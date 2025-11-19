#include "uwb.h"
#include "deca_device_api.h"
#include "deca_regs.h"
#include <stdio.h>
#include <stdint.h>

#define FRAME_LEN_MAX 127
static uint16 frame_len = 0;

void UWB_ReceiveLoop(void)
{
    uint8_t rx_buffer[FRAME_LEN_MAX];
    uint32_t status_reg = 0;

    while (1)
    {
        // 1. Clear các cờ lỗi và sự kiện cũ
        dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_ERR | SYS_STATUS_RXFCG);

        // 2. Bật chế độ nhận ngay lập tức
        dwt_rxenable(DWT_START_RX_IMMEDIATE);

        // 3. Poll chờ sự kiện
        do
        {
            status_reg = dwt_read32bitreg(SYS_STATUS_ID);
        }
        while (!(status_reg & (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_ERR)));

        // 4. Nếu nhận thành công
        if (status_reg & SYS_STATUS_RXFCG)
        {
            // Đọc độ dài frame
            frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFL_MASK_1023;

            if (frame_len <= FRAME_LEN_MAX)
            {
                dwt_readrxdata(rx_buffer, frame_len, 0);
            }

            // Clear flag good frame
            dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);
        }
        else
        {
            // Có lỗi -> clear + reset RX
            dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_ERR);
            dwt_rxreset();
        }
    }
}
