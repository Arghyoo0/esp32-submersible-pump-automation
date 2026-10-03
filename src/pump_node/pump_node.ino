#define BLYNK_TEMPLATE_ID "template id"
#define BLYNK_TEMPLATE_NAME "Smart Water Tank"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"

#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h> 
#include <BlynkSimpleEsp32.h>

// ==========================================
// 📍 HARDWARE WIRING
// ==========================================
const int RELAY_START = 19; // IN1 -> Green Start Button
const int RELAY_STOP  = 18; // IN2 -> Red Stop Button

// ==========================================
// 📡 NETWORK CREDENTIALS
// ==========================================
char auth[] = "YOUR_BLYNK_AUTH_TOKEN";
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";

// 🎯 YOUR ACTUAL ROOF ESP MAC ADDRESS
uint8_t roofAddress[] = {0xEC, 0xE3, 0x34, 0x1B, 0x0D, 0x64}; 

// ==========================================
// 🧠 SYSTEM VARIABLES
// ==========================================
int waterLevel = 0;
bool isPumpRunning = false;
volatile bool newLevelReceived = false; 

// 🛡️ Software Armor Counters
int lowConfidenceCounter = 0;
int highConfidenceCounter = 0;

// ==========================================
// 📦 ESP-NOW DATA PACKAGES
// ==========================================
// What we RECEIVE from the roof
typedef struct struct_message {
    int percentage;
} struct_message;
struct_message incomingData;

// What we SEND back to the roof
typedef struct reply_message {
    bool isPumpRunning;
} reply_message;
reply_message replyData;

esp_now_peer_info_t peerInfo;

// ==========================================
// ⚙️ RELAY CONTROL LOGIC
// ==========================================
void sendStatusToRoof() {
    replyData.isPumpRunning = isPumpRunning;
    esp_now_send(roofAddress, (uint8_t *) &replyData, sizeof(replyData));
}

void startPump() {
    if (isPumpRunning) return; 
    
    Serial.println("⚙️ Action: Starting Pump (3-Second Pulse)...");
    digitalWrite(RELAY_START, LOW);  
    delay(3000);                     
    digitalWrite(RELAY_START, HIGH); 
    
    isPumpRunning = true;
    sendStatusToRoof();
    
    // Sync UI if online
    if (Blynk.connected()) {
        Blynk.virtualWrite(V1, 1); 
        Blynk.virtualWrite(V2, 1); 
    }
}

void stopPump() {
    if (!isPumpRunning) return; 
    
    Serial.println("⚙️ Action: Stopping Pump (1-Second Pulse)...");
    digitalWrite(RELAY_STOP, LOW);   
    delay(1000);                     
    digitalWrite(RELAY_STOP, HIGH);  
    
    isPumpRunning = false;
    sendStatusToRoof();
    
    // Sync UI if online
    if (Blynk.connected()) {
        Blynk.virtualWrite(V1, 0); 
        Blynk.virtualWrite(V2, 0); 
    }
}

// ==========================================
// 📥 ESP-NOW RECEIVE CALLBACK
// ==========================================
void OnDataRecv(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int len) {
    memcpy(&incomingData, data, sizeof(incomingData));
    waterLevel = incomingData.percentage;
    newLevelReceived = true; 
}

// ==========================================
// 🌐 BLYNK LOGIC
// ==========================================
BLYNK_CONNECTED() {
    Serial.println("SUCCESS: Connected to Blynk Cloud!");
    Blynk.virtualWrite(V1, isPumpRunning ? 1 : 0);
    Blynk.virtualWrite(V2, isPumpRunning ? 1 : 0);
}

// App Buttons
BLYNK_WRITE(V1) {
    if (param.asInt() == 1) startPump();
    else stopPump();
}
BLYNK_WRITE(V2) {
    if (param.asInt() == 1) startPump();
    else stopPump();
}

// ==========================================
// 🚀 SETUP
// ==========================================
void setup() {
    Serial.begin(115200);
    
    // 1. Initialize Relays to OFF (HIGH is OFF for these boards)
    pinMode(RELAY_START, OUTPUT);
    pinMode(RELAY_STOP, OUTPUT);
    digitalWrite(RELAY_START, HIGH); 
    digitalWrite(RELAY_STOP, HIGH);  

    // 2. Start Wi-Fi
    WiFi.mode(WIFI_STA);
    
    // 3. OFFLINE FIX: Force the channel to 8 immediately (matches Roof fallback)
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(8, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);

    // 4. Try connecting to router (Non-blocking)
    WiFi.begin(ssid, pass);

    // 5. Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW Init Failed!");
        return;
    }
    
    esp_now_register_recv_cb(OnDataRecv);

    // 6. Register Roof as a peer
    memcpy(peerInfo.peer_addr, roofAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);

    // 7. Setup Blynk
    Blynk.config(auth); 
    
    Serial.println("Pump ESP Booted. Waiting for Roof Data...");
}

// ==========================================
// 🔄 MAIN LOOP
// ==========================================
void loop() {
    // Keep Blynk running if we have internet
    if (WiFi.status() == WL_CONNECTED) {
        Blynk.run();
    }

    // Process new data from the roof safely
    if (newLevelReceived) {
        newLevelReceived = false; 
        
        Serial.print("📥 Incoming ESP-NOW -> Water Level: ");
        Serial.print(waterLevel);
        Serial.println("%");
        
        // Update App Gauge
        if (Blynk.connected()) {
            Blynk.virtualWrite(V0, waterLevel);
        }

        // ==========================================
        // 🛡️ SOFTWARE ARMOR (WAVE/SLOSH FILTER)
        // ==========================================
        if (waterLevel <= 20) {
            lowConfidenceCounter++;
            highConfidenceCounter = 0; 
        } 
        else if (waterLevel >= 80) {
            highConfidenceCounter++;
            lowConfidenceCounter = 0; 
        } 
        else {
            // Water is in the middle safe zone, reset counters
            lowConfidenceCounter = 0;
            highConfidenceCounter = 0;
        }

        // 🎯 FIRE ACTIONS (Only if 100% confident)
        if (lowConfidenceCounter >= 3) {
            startPump();
            lowConfidenceCounter = 0; // Reset after firing
        } 
        else if (highConfidenceCounter >= 3) {
            stopPump();
            highConfidenceCounter = 0; // Reset after firing
        }
    }
}
