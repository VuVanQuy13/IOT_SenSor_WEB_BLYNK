# 🌍 Hệ thống giám sát môi trường IoT với ESP32 + WebSocket + Blynk

## 📌 Giới thiệu
Đây là một **hệ thống IoT giám sát môi trường** sử dụng **ESP32**.  
Hệ thống có khả năng đo **nhiệt độ, độ ẩm và mật độ bụi**, sau đó hiển thị dữ liệu theo thời gian thực trên:  
- **Web Dashboard** (HTML/CSS + WebSocket) chạy trực tiếp trên ESP32.  
- **Ứng dụng Blynk IoT** để giám sát và điều khiển từ xa qua Internet.  

Hệ thống hỗ trợ **chế độ Tự động/Thủ công** và cho phép điều chỉnh ngưỡng cảnh báo ngay trên **giao diện web** hoặc **app Blynk**. Giá trị ngưỡng được lưu trong **Preferences (EEPROM giả lập của ESP32)** để không bị mất sau khi khởi động lại.

---

## ⚙️ Tính năng
- 🌡 Đo **nhiệt độ & độ ẩm** bằng cảm biến **DHT11**  
- 🌫 Đo **nồng độ bụi** bằng cảm biến **Sharp GP2Y10** (tự viết thư viện C++)  
- 📡 **Dashboard Web thời gian thực** (WebSocket + SPIFFS + Bootstrap)  
- 📱 Kết nối với **Blynk IoT Cloud** để theo dõi & điều khiển từ xa  
- 🔧 **Điều chỉnh ngưỡng cảnh báo** qua slider trên web hoặc Blynk app  
- 💾 Lưu trữ cài đặt bằng **Preferences** (không mất sau reset)  
- 🛡 Tích hợp **Watchdog Timer (WDT)** và **WiFi auto-reconnect** để hệ thống ổn định  
- 💡 Chế độ **Tự động/Thủ công** điều khiển LED cảnh báo  

---

## 🛠 Công nghệ & Công cụ
- **Ngôn ngữ:** C, C++ (Arduino Framework)  
- **Vi điều khiển:** ESP32  
- **Nền tảng IoT:** Blynk IoT Cloud  
- **Web:** HTML, CSS, Bootstrap, FontAwesome, WebSocket  
- **Cảm biến:** DHT11, Sharp GP2Y10  
- **Thư viện:** ESPAsyncWebServer, Arduino_JSON, SPIFFS, Preferences, esp_task_wdt  
- **Công cụ phát triển:** Arduino IDE / PlatformIO, GitHub  

---

## 📂 Cấu trúc dự án
├── firmware/ # Code ESP32  
├── data/ # File HTML, CSS, JS cho SPIFFS  
└── README.md    

## Giao diện Web Dashboard:  
<img width="1917" height="913" alt="image" src="https://github.com/user-attachments/assets/ed7c83a6-ef63-4d88-a2bc-2574b901cb92" />  

## Giao diện BLYNK:  
<img width="1617" height="742" alt="image" src="https://github.com/user-attachments/assets/f35b83a9-2b18-497a-9ec4-69e8e346aa2f" />  


## Altium:
- **SCH: 
<img width="1081" height="717" alt="image" src="https://github.com/user-attachments/assets/3a977b6c-a00d-4419-afe5-d5c3162974ae" />

- **PCB:
<img width="1119" height="712" alt="image" src="https://github.com/user-attachments/assets/37066228-31f4-4d04-9528-03fb1f0f843e" />

## Mạch Hoàn Thiện:
![ALtium](https://github.com/user-attachments/assets/f8a46e01-963e-4a49-8875-94911fc64c47)

![Duoi](https://github.com/user-attachments/assets/4c70fc32-14a1-4549-aa13-a8ec93be3b7f)


