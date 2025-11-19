#include "app_init.h"
#include "deca_device_api.h"
#include "deca_regs.h"
#include "deca_types.h"
#include "port.h"

// Cấu hình mặc định cho DWM1000
static dwt_config_t config = {
    5,                  // Channel 5
    DWT_PRF_64M,        // Pulse repetition frequency 64 MHz
    DWT_PLEN_1024,      // Preamble length 1024
    DWT_PAC32,          // PAC size 32
    9,                  // TX preamble code
    9,                  // RX preamble code
    1,                  // Non-standard SFD
    DWT_BR_110K,        // Data rate 110 kbps
    DWT_PHRMODE_STD,    // PHY header mode standard
    (1025 + 64 - 32)    // SFD timeout
};

// Hàm khởi tạo UWB
void UWB_Init(void)
{
    port_set_dw1000_slowrate();

    if (dwt_initialise(DWT_LOADNONE) == DWT_ERROR)
    {
        while (1); // Lỗi khởi tạo
    }

    port_set_dw1000_fastrate();
    dwt_configure(&config);
}
