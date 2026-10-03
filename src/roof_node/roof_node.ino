#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h> // Required for channel auto-scanning

// ==========================================
// 📍 HARDWARE WIRING
// ==========================================
const int TRIG_PIN = 27; // Plugged into P27
const int ECHO_PIN = 26; // Plugged into P26

// ==========================================
// 📡 NETWORK & TARGET SETTINGS
// ==========================================
// The exact MAC Address of your Pump ESP
uint8_t pumpAddress[] = {0x44, 0x1D, 0x64, 0xBD, 0x17, 0x1C};

// Your router SSID (Used to auto-find the correct Wi-Fi channel)
const char* ssid = "YOUR SSID";

// ==========================================
// 📦 ESP-NOW DATA PACKAGES
// ==========================================
// What we SEND to the pump
typedef struct struct_message {
    int percentage;
} struct_message;
struct_message myData;

// What we RECEIVE from the pump
typedef struct incoming_message {
    bool isPumpRunning;
} incoming_message;
incoming_message pumpData;

esp_now_peer_info_t peerInfo;

// ==========================================
// 🔍 AUTO-CHANNEL SCANNER
// ==========================================
int32_t getWiFiChannel(const char *ssid) {
    if (int32_t n = WiFi.scanNetworks()) {
        for (uint8_t i = 0; i < n; i++) {
            if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
                return WiFi.channel(i);
            }
        }
    }
    return 0; // Network not found
}

// ==========================================
// 🚀 CALLBACK: WHEN WE SEND DATA
// ==========================================
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Delivery to Pump: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "SUCCESS" : "FAIL");
}

// ==========================================
// 📥 CALLBACK: WHEN WE RECEIVE DATA
// ==========================================
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    memcpy(&pumpData, incomingData, sizeof(pumpData));
    Serial.print("Update from Pump Room: ");
    Serial.println(pumpData.isPumpRunning ? "PUMP IS ON 🟢" : "PUMP IS OFF 🔴");
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    Serial.println("\n--- ROOF SENSOR BOOTING ---");

    // 1. Start Wi-Fi in Station Mode
    WiFi.mode(WIFI_STA);
    
    // 2. Scan for DJhome's current channel
    Serial.println("Scanning for DJhome router...");
    int32_t channel = getWiFiChannel(ssid);
    
    if (channel == 0) {
        Serial.println("Router offline! Defaulting to fallback Channel 8.");
        channel = 8;
    } else {
        Serial.print("Router found! Locking ESP-NOW to Channel: ");
        Serial.println(channel);
    }

    // 3. Force ESP32 onto the matching channel
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);

    // 4. Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("CRITICAL ERROR: ESP-NOW Initialization Failed");
        return;
    }
    
    // 5. Register Both Callbacks (Send and Receive)
    esp_now_register_send_cb((esp_now_send_cb_t)OnDataSent);
    esp_now_register_recv_cb((esp_now_recv_cb_t)OnDataRecv);

    // 6. Register the Pump ESP as a peer
    memcpy(peerInfo.peer_addr, pumpAddress, 6);
    peerInfo.channel = 0; // 0 means "use the channel we just locked onto"
    peerInfo.encrypt = false;
    
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add Pump ESP as peer");
        return;
    }
    
    Serial.println("System Ready. Reading sensor...");
}

void loop() {
    // 1. Fire the Ultrasonic Sensor
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    // 2. Read the Echo
    long duration = pulseIn(ECHO_PIN, HIGH);
    int distance = duration * 0.034 / 2; // Convert time to cm
    
    // =================================================================
    // ⚠️ REAL TANK CALIBRATION (ROOF TESTED) ⚠️
    // =================================================================
    int tankDepth = 85; // Distance in cm when tank is EMPTY (0%)
    int safeTop = 30;   // Distance in cm when tank is FULL (100%)
    // =================================================================
    
    // 3. Calculate Percentage based on calibration
    int waterLevel = map(distance, tankDepth, safeTop, 0, 100);
    waterLevel = constrain(waterLevel, 0, 100); // Keep it exactly between 0-100%

    // Print to Serial Monitor so you can see what's happening
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.print(" cm  -->  Tank is ");
    Serial.print(waterLevel);
    Serial.println("% Full");

    // 4. Package the data and fire it to the Pump ESP!
    myData.percentage = waterLevel;
    esp_now_send(pumpAddress, (uint8_t *) &myData, sizeof(myData));

    // Wait 2 seconds before taking the next reading
    delay(2000); 
}
