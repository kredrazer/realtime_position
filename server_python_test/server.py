import socket
import threading
import tkinter as tk
from tkinter import scrolledtext
from tkinter import simpledialog

# =============================================================================
# CẤU HÌNH BAN ĐẦU
# =============================================================================

DEFAULT_PORT = 12345 
RECEIVE_IP = "0.0.0.0" 
BUFFER_SIZE = 1024 

# =============================================================================
# LOGIC LẮNG NGHE UDP (Trong Thread riêng)
# =============================================================================

class UdpListener(threading.Thread):
    def __init__(self, port, callback):
        super().__init__()
        self.port = port
        self.callback = callback
        self.running = False
        self.sock = None

    def run(self):
        self.running = True
        try:
            # 1. Tạo Socket
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            # Thiết lập timeout để thread có thể thoát khi self.running = False
            self.sock.settimeout(0.5) 

            # 2. Bind Socket
            self.sock.bind((RECEIVE_IP, self.port))
            self.callback(f"✅ Đang lắng nghe trên {RECEIVE_IP}:{self.port}...", 'status')

            # 3. Vòng lặp nhận dữ liệu
            while self.running:
                try:
                    data, addr = self.sock.recvfrom(BUFFER_SIZE)
                    
                    # Gọi lại hàm hiển thị trên GUI thread
                    self.callback(data, 'data', addr)
                    
                except socket.timeout:
                    # Bỏ qua timeout, tiếp tục kiểm tra self.running
                    continue
                except Exception as e:
                    if self.running:
                        self.callback(f"Lỗi Socket: {e}", 'error')
                        self.running = False
                    break

        except Exception as e:
            self.callback(f"Lỗi khởi tạo socket: {e}", 'error')
            self.running = False

        finally:
            if self.sock:
                self.sock.close()
            self.callback("❌ Lắng nghe UDP đã dừng.", 'status')

    def stop(self):
        self.running = False
        # Cần đóng socket để giải phóng port và thoát thread
        # Việc đóng socket sẽ được thực hiện trong khối finally của run()
        
# =============================================================================
# GIAO DIỆN NGƯỜI DÙNG (GUI APP)
# =============================================================================

class UdpViewerApp:
    def __init__(self, master):
        self.master = master
        master.title("UWB UDP Viewer (Hoang Quan)")
        master.geometry("750x550")

        self.listener = None
        self.is_running = False
        
        # --- 1. Control Frame (Cấu hình và Nút bấm) ---
        self.control_frame = tk.Frame(master, padx=10, pady=10)
        self.control_frame.pack(fill='x')

        tk.Label(self.control_frame, text="Port:").pack(side='left', padx=(0, 5))
        self.port_var = tk.StringVar(value=str(DEFAULT_PORT))
        self.port_entry = tk.Entry(self.control_frame, width=10, textvariable=self.port_var)
        self.port_entry.pack(side='left', padx=(0, 15))
        
        self.start_button = tk.Button(self.control_frame, text="Start Listening", command=self.start_listener, bg='green', fg='white')
        self.start_button.pack(side='left', padx=5)
        
        self.stop_button = tk.Button(self.control_frame, text="Stop Listening", command=self.stop_listener, state=tk.DISABLED, bg='red', fg='white')
        self.stop_button.pack(side='left', padx=5)
        
        self.clear_button = tk.Button(self.control_frame, text="Clear Log", command=self.clear_log)
        self.clear_button.pack(side='left', padx=(30, 5))

        # --- 2. Status Bar ---
        self.status_bar = tk.Label(master, text="Chưa khởi động", bd=1, relief=tk.SUNKEN, anchor='w')
        self.status_bar.pack(side=tk.BOTTOM, fill='x')

        # --- 3. Log Area (Vùng hiển thị bản tin) ---
        tk.Label(master, text="UDP Received Data Log:").pack(padx=10, anchor='w')
        self.log_area = scrolledtext.ScrolledText(master, wrap=tk.WORD, width=80, height=25, font=('Consolas', 10))
        self.log_area.pack(padx=10, pady=5, fill='both', expand=True)
        self.log_area.config(state=tk.DISABLED)
        
        # Xử lý đóng cửa sổ
        master.protocol("WM_DELETE_WINDOW", self.on_closing)

    def display_data(self, message, message_type, addr=None):
        # Hàm được gọi từ thread phụ, đảm bảo cập nhật trên main GUI thread
        self.log_area.config(state=tk.NORMAL)
        
        if message_type == 'status' or message_type == 'error':
            color = 'blue' if message_type == 'status' else 'red'
            self.status_bar.config(text=message, fg=color)
            self.log_area.insert(tk.END, f"--- {message}\n", message_type)
        
        elif message_type == 'data':
            # Xử lý hiển thị bản tin
            data_bytes = message
            data_len = len(data_bytes)
            hex_data = data_bytes.hex()
            formatted_hex = ' '.join([hex_data[i:i+2] for i in range(0, data_len * 2, 2)])

            self.log_area.insert(tk.END, f"[{addr[0]}:{addr[1]}] | Length: {data_len} Bytes\n", 'header')
            self.log_area.insert(tk.END, f"HEX: {formatted_hex}\n", 'hex')
            self.log_area.insert(tk.END, f"RAW: {data_bytes}\n\n", 'raw')
            
        self.log_area.see(tk.END)
        self.log_area.config(state=tk.DISABLED)
        
        # Cấu hình màu cho log
        self.log_area.tag_config('header', foreground='green', font=('Consolas', 10, 'bold'))
        self.log_area.tag_config('hex', foreground='black')
        self.log_area.tag_config('raw', foreground='gray')
        self.log_area.tag_config('error', foreground='red', font=('Consolas', 10, 'bold'))

    def start_listener(self):
        if self.is_running:
            return
            
        try:
            port = int(self.port_var.get())
            if port < 1024 or port > 65535:
                raise ValueError("Port không hợp lệ")

            self.listener = UdpListener(port, self.display_data)
            self.listener.start()
            self.is_running = True
            
            self.port_entry.config(state=tk.DISABLED)
            self.start_button.config(state=tk.DISABLED)
            self.stop_button.config(state=tk.NORMAL)

        except ValueError as e:
            self.status_bar.config(text=f"Lỗi cấu hình: {e}", fg='red')

    def stop_listener(self):
        if self.listener:
            self.listener.stop()
            self.is_running = False
            self.listener = None 
            
            self.port_entry.config(state=tk.NORMAL)
            self.start_button.config(state=tk.NORMAL)
            self.stop_button.config(state=tk.DISABLED)

    def clear_log(self):
        self.log_area.config(state=tk.NORMAL)
        self.log_area.delete(1.0, tk.END)
        self.log_area.config(state=tk.DISABLED)

    def on_closing(self):
        if self.is_running:
            self.stop_listener()
        self.master.destroy()

if __name__ == "__main__":
    root = tk.Tk()
    app = UdpViewerApp(root)
    root.mainloop()