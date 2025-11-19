#include <WiFi.h>
#include <WiFiUdp.h>

// =============================================================================
// I. CẤU HÌNH HỆ THỐNG (BẮT BUỘC PHẢI THAY ĐỔI)
// =============================================================================

const char* ssid = "TEN_WIFI_CUA_BAN";         // <-- ĐIỀN TÊN WIFI CỦA BẠN
const char* password = "MAT_KHAU_WIFI";        // <-- ĐIỀN MẬT KHẨU WIFI CỦA BẠN
const IPAddress remoteIP(192, 168, 1, 100);    // <-- ĐỊA CHỈ IP CỦA MÁY TÍNH NHẬN UDP
const unsigned int remotePort = 12345;         // <-- PORT UDP MỞ TRÊN MÁY TÍNH
const long uart_baudrate = 115200;             // Baud rate phải khớp với STM32/Anchor

// UART Pins cho ESP32 (Sử dụng Serial2)
#define RXD2 16 // GPIO 16 (Kết nối với TX của STM32)
#define TXD2 17 // GPIO 17 (Có thể không cần nối)

// Kích thước buffer tối đa (Đảm bảo lớn hơn frame lớn nhất)
#define MAX_BUFFER_SIZE 32 

// Thời gian chờ tối đa (ms) giữa các byte trước khi quyết định gửi buffer
#define UART_TIMEOUT_MS 5 

// =============================================================================
// II. BIẾN GLOBAL VÀ KHAI BÁO HÀM
// =============================================================================

WiFiUDP Udp;
uint8_t rx_buffer[MAX_BUFFER_SIZE];
size_t rx_buffer_idx = 0;
unsigned long last_byte_time = 0;

void wifiSetup();
void forward_to_udp(const uint8_t* data, size_t len);

// =============================================================================
// III. TRIỂN KHAI HÀM
// =============================================================================

void wifiSetup() {
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

/**
 * @brief Gửi gói tin qua UDP
 */
void forward_to_udp(const uint8_t* data, size_t len) {
    if (len == 0) return;

    Udp.beginPacket(remoteIP, remotePort);
    Udp.write(data, len);
    if (Udp.endPacket()) {
        Serial.print("Forwarded ");
        Serial.print(len);
        Serial.println(" bytes via UDP.");
    } else {
        Serial.println("Error: Failed to send UDP packet.");
    }
    // Reset buffer sau khi gửi
    rx_buffer_idx = 0; 
}

// =============================================================================
// IV. SETUP VÀ LOOP CHÍNH
// =============================================================================

void setup() {
    // 1. Serial Monitor
    Serial.begin(115200); 
    
    // 2. UART Giao tiếp với STM32 (Serial2)
    Serial2.begin(uart_baudrate, SERIAL_8N1, RXD2, TXD2);
    Serial.println("UART configured on Serial2.");

    // 3. Kết nối WiFi
    wifiSetup();
}

void loop() {
    // --- 1. Xử lý nhận byte UART ---
    while (Serial2.available()) {
        uint8_t incomingByte = Serial2.read();
        
        // Chỉ lưu nếu buffer còn chỗ
        if (rx_buffer_idx < MAX_BUFFER_SIZE) {
            rx_buffer[rx_buffer_idx++] = incomingByte;
            last_byte_time = millis(); // Cập nhật thời gian nhận byte cuối
        } else {
            // Buffer đầy, gửi đi ngay lập tức
            Serial.println("Warning: Buffer full. Forcing UDP send.");
            forward_to_udp(rx_buffer, rx_buffer_idx);
            // Sau khi gửi, buffer đã được reset (rx_buffer_idx = 0)
        }
    }

    // --- 2. Kiểm tra Timeout ---
    // Nếu buffer có dữ liệu và đã quá thời gian chờ từ byte cuối cùng
    if (rx_buffer_idx > 0 && (millis() - last_byte_time > UART_TIMEOUT_MS)) {
        Serial.println("Timeout occurred. Sending partial frame.");
        forward_to_udp(rx_buffer, rx_buffer_idx);
    }
}