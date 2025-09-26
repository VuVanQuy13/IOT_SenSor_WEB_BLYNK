#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>
#include <DHT.h>
#include "SharpGP2Y10.h"
#include <Preferences.h>
Preferences prefs;
#include "esp_task_wdt.h"
#define WDT_TIMEOUT 15

int errorCount = 0;

// =========== Blynk =============
#define BLYNK_TEMPLATE_ID "TMPL6Jz6zCaGZ"
#define BLYNK_TEMPLATE_NAME "IOT Measure Sensor"
#define BLYNK_AUTH_TOKEN "kNDkR1p17mJJ6lcBspS4ivnyDhQnqiA9"

#include <BlynkSimpleEsp32.h>
// #include <TimeLib.h>
// #include <WidgetRTC.h>

// ========== End Blynk ============= 

// ========= SenSor ============
unsigned long lastSend = 0;
const unsigned long SEND_INTERVAL = 2000; // ms
double t = -999;
double h = -999;
double dust = -999; 

// Ngưỡng cài đặt
int thresold_NHIETDO = 35;   // độ C
int thresold_DOAM = 80;      // %
int thresold_BUI = 100;    // µg/m3

// LED
#define LED_NhietDo 22
#define LED_DoAm 19
#define LED_Bui 4

// DHT11
#define DHTtype DHT11
#define DHTpin 14
DHT dht(DHTpin , DHTtype);

// SharpGP2Y10
#define v0Pin 32
#define ledPin 26
SharpGP2Y10 dustSensor(v0Pin , ledPin);

void readSensor() {
  t = dht.readTemperature();
  h = dht.readHumidity();
  dust = dustSensor.readDustDensity();

  if(isnan(t)) t = -999;
  if(isnan(h)) h = -999;
  if (dust < 0 || dust > 1000) dust = -999;
}

// =============== End Sensor ==================


// =============== Blynk ===============
BlynkTimer timer;
bool manualMode = false;  // false = Auto, true = Manual


void sendSensor() {
  readSensor();

  if(isnan(t) && isnan(h))
  {
    // Serial.println("Failed to read from DHT and SharpGP2Y10 sensor !");
    // return;
    t = 50;
    h = 50;
    dust = 50;
  }

  Serial.println("Gia tri doc tu cam bien gui len Blynk: ");
  Serial.println("Temp: " + String(t));
  Serial.println("Hum: " + String(h));
  Serial.println("Dust: " + String(dust));

  Blynk.virtualWrite(V0 , t);
  Blynk.virtualWrite(V1 , h);
  Blynk.virtualWrite(V2 , dust);
}
// =========== End Blynk =================


// =========== Wifi ====================
const char* ssid = "SN44";
const char* password = "mothaiba";

void wifiEvent(WiFiEvent_t event) {
  switch (event)
  {
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("Wifi disconnect");
      WiFi.reconnect();
      break;
    
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.print("Wifi connected , IP: ");
      Serial.println(WiFi.localIP());
      break;

    default:
      break;
  }
}

void initWifi() {
  WiFi.mode(WIFI_STA);            // AP STA AP_STA
  WiFi.begin(ssid , password);
  Serial.println("Connecting to Wifi ...");
  WiFi.onEvent(wifiEvent);
}
// ================== End Wifi =======================


// ================= SPIFFS ==================
void initSPIFFS() {
  if(!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS!");
  } 
  else {
    Serial.println("SPIFFS mounted successfully.");
  }
}
// ================== End SPIFFS ===================


// ========= Server , WebSocket ==========
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// ============= WebSocket ===============
JSONVar responeSensor;                           // Nếu gửi JSON dạng thủ công thì k cần dùng
String message = "";
String sensor1 = "0";
String sensor2 = "0";
String sensor3 = "0";

// Gửi giao diện 
String getSliderValues() {
  responeSensor["sensor1"] = String(sensor1);
  responeSensor["sensor2"] = String(sensor2);
  responeSensor["sensor3"] = String(sensor3);

  String jsonString = JSON.stringify(responeSensor);
  return jsonString;
}

// Gửi cho người số 2 3 4 5 6 
void notifyClients(String respone) {
  ws.textAll(respone);
}

void handleWebSocketRequest(void * arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*) arg;

  if(info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    // Xử lí khi nhận được request
    char msg[len + 1];        
    memcpy(msg, data, len);   // Copy dữ liệu từ data sang msg
    msg[len] = '\0';          
    message = String(msg);    

    if(message.indexOf("1s") >= 0) {
      sensor1 = message.substring(2);      
      thresold_NHIETDO = sensor1.toInt();
      Serial.printf("Giá trị 1: %d" , thresold_NHIETDO);
      Blynk.virtualWrite(V10 , thresold_NHIETDO);
      prefs.putInt("NhietDo" , thresold_NHIETDO);
      Serial.println(getSliderValues());

      notifyClients(getSliderValues()); 
    }

    if(message.indexOf("2s") >= 0) {
      sensor2 = message.substring(2);
      thresold_DOAM = sensor2.toInt();
      Serial.printf("Giá trị 2: %d" , thresold_DOAM);
      Blynk.virtualWrite(V11 , thresold_DOAM);
      prefs.putInt("DoAm" , thresold_DOAM);
      Serial.println(getSliderValues());
      notifyClients(getSliderValues()); 
    }

    if(message.indexOf("3s") >= 0) {
      sensor3 = message.substring(2);
      thresold_BUI = sensor3.toInt();
      Serial.printf("Giá trị 3: %d" , thresold_BUI);
      Blynk.virtualWrite(V12 , thresold_BUI);
      prefs.putInt("DoBui" , thresold_BUI);
      Serial.println(getSliderValues());
      notifyClients(getSliderValues()); 
    }

    // Ban đầu trình duyệt khi chưa có dữ liệu
    if(strcmp((char *)data , "getValues") == 0) {
      Serial.printf("Gia tri ban dau %s\n" , getSliderValues());
      notifyClients(getSliderValues());
    }
  }

}

void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  switch (type)
  {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s" , client->id() , client->remoteIP().toString().c_str());
    sensor1 = String(thresold_NHIETDO);
    sensor2 = String(thresold_DOAM);
    sensor3 = String(thresold_BUI);
    notifyClients(getSliderValues());
    break;
    
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;

  case WS_EVT_DATA:
    handleWebSocketRequest(arg , data , len);
    break;
  
  default:
    break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}



void setup() {
  Serial.begin(9600);
  delay(1000);
  esp_task_wdt_init(WDT_TIMEOUT , true);
  esp_task_wdt_add(NULL);

  // Khởi tạo LED
  pinMode(LED_NhietDo, OUTPUT);
  pinMode(LED_DoAm, OUTPUT);
  pinMode(LED_Bui, OUTPUT);

  dht.begin();

  prefs.begin("Gia tri nguong" ,false);
  thresold_NHIETDO = prefs.getInt("NhietDo", 35);
  thresold_DOAM = prefs.getInt("DoAm", 80);
  thresold_BUI = prefs.getInt("Bui", 100);

  initWifi();
  initSPIFFS();

  initWebSocket();
  
  // ========== render web ===========
  server.on("/" ,HTTP_GET , [](AsyncWebServerRequest *request) {
    if(SPIFFS.exists("/index.html")) {
      request->send(SPIFFS , "/index.html" , "text/html");
    } else {
      request->send(200 , "text/html" , "<h1>No index.html found</h1>");
    }
  });
  server.serveStatic("/" , SPIFFS , "/");
  server.begin();

  // ======== Blynk ==========
  Blynk.begin(BLYNK_AUTH_TOKEN , ssid , password);
  timer.setInterval(3000L , sendSensor);
}

void loop() {

  // ======== Blynk ==========
  Blynk.run();
  timer.run();
  // ======= end Blynk =========


  unsigned long now = millis();
  readSensor();


  // send for clients
  if(now - lastSend >= SEND_INTERVAL) {
    lastSend = now;

    JSONVar data;
    data["temperature"] = t;
    data["humidity"] = h;
    data["dust"] = dust;
    String payload = JSON.stringify(data);

    ws.textAll(payload);
    Serial.println(payload);

    ws.cleanupClients();
  }

  //// Relay on/off (chỉ chạy khi ở chế độ Auto)
  if (!manualMode && t!=-999 && h!=-999 && dust!=-999) {
    if(t > thresold_NHIETDO) {
      digitalWrite(LED_NhietDo , HIGH);
      Blynk.virtualWrite(V3 , HIGH);
    } else {
      digitalWrite(LED_NhietDo , LOW);
      Blynk.virtualWrite(V3 , LOW);
    }
    
    if(h < thresold_DOAM) {
      digitalWrite(LED_DoAm , HIGH);
      Blynk.virtualWrite(V4 , HIGH);
    } else {
      digitalWrite(LED_DoAm , LOW);
      Blynk.virtualWrite(V4 , LOW);
    }
    
    if(dust > thresold_BUI) {
      digitalWrite(LED_Bui , HIGH);
      Blynk.virtualWrite(V5 , HIGH);
    } else {
      digitalWrite(LED_Bui , LOW);
      Blynk.virtualWrite(V5 , LOW);
    }
  }

  esp_task_wdt_reset();
}

BLYNK_WRITE(V6) {
  manualMode = true; 

  int pinValue = param.asInt();
  if(pinValue == 1) {
    digitalWrite(LED_NhietDo , HIGH);
    Blynk.virtualWrite(V3 , HIGH);
  } else {
    digitalWrite(LED_NhietDo , LOW);
    Blynk.virtualWrite(V3 , LOW);
  }
}

BLYNK_WRITE(V7) {
  manualMode = true; 

  int pinValue = param.asInt();
  if(pinValue == 1) {
    digitalWrite(LED_DoAm , HIGH);
    Blynk.virtualWrite(V4 , HIGH);
  } else {
    digitalWrite(LED_DoAm , LOW);
    Blynk.virtualWrite(V4 , LOW);
  }
}

BLYNK_WRITE(V8) {
  manualMode = true; 

  int pinValue = param.asInt();
  if(pinValue == 1) {
    digitalWrite(LED_Bui , HIGH);
    Blynk.virtualWrite(V5 , HIGH);
  } else {
    digitalWrite(LED_Bui , LOW);
    Blynk.virtualWrite(V5 , LOW);
  }
}

BLYNK_WRITE(V9) {
  int mode = param.asInt();
  if (mode == 1) {
    manualMode = false; 
    Serial.println("Mode to AUTO mode");
  } else {
    manualMode = true;  
    Serial.println("Mode to MANUAL mode");
  }
}

BLYNK_WRITE(V10) {
  thresold_NHIETDO = param.asInt();
  sensor1 = String(thresold_NHIETDO);
  prefs.putInt("NhietDo", thresold_NHIETDO);

  // Gửi lại cho web
  notifyClients(getSliderValues());

  Serial.printf("Slider V10 set threshold: %d\n", thresold_NHIETDO);
}

BLYNK_WRITE(V11) {
  thresold_DOAM = param.asInt();
  sensor2 = String(thresold_DOAM);
  prefs.putInt("DoAm", thresold_DOAM);

  notifyClients(getSliderValues());
  Serial.printf("Slider V11 set threshold: %d\n", thresold_DOAM);
}

BLYNK_WRITE(V12) {
  thresold_BUI = param.asInt();
  sensor3 = String(thresold_BUI);
  prefs.putInt("Bui", thresold_BUI);

  notifyClients(getSliderValues());
  Serial.printf("Slider V12 set threshold: %d\n", thresold_BUI);
}

