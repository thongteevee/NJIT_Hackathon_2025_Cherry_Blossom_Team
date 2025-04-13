#include "ESP32_NOW.h"
#include "WiFi.h"

#define SENSOR_PIN 4         // PIR sensor pin
#define INDICATOR_PIN 17     // LED pin
#define ESPNOW_WIFI_CHANNEL 6
#define CLEAR_DELAY 1000     // milliseconds  //100 is better

class ESP_NOW_Broadcast_Peer : public ESP_NOW_Peer {
public:
  ESP_NOW_Broadcast_Peer(uint8_t channel, wifi_interface_t iface, const uint8_t *lmk)
    : ESP_NOW_Peer(ESP_NOW.BROADCAST_ADDR, channel, iface, lmk) {}

  ~ESP_NOW_Broadcast_Peer() {
    remove();
  }

  bool begin() {
    if (!ESP_NOW.begin() || !add()) {
      log_e("Failed to initialize ESP-NOW or register broadcast peer");
      return false;
    }
    return true;
  }

  bool send_message(const char *msg) {
    if (!send((uint8_t *)msg, strlen(msg) + 1)) {
      log_e("Failed to send ESP-NOW message");
      return false;
    }
    return true;
  }
};

ESP_NOW_Broadcast_Peer broadcast_peer(ESPNOW_WIFI_CHANNEL, WIFI_IF_STA, NULL);

bool motionDetected = false;
unsigned long lastMotionTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_PIN, INPUT);
  pinMode(INDICATOR_PIN, OUTPUT);

  // Wi-Fi setup for ESP-NOW
  WiFi.mode(WIFI_STA);
  WiFi.setChannel(ESPNOW_WIFI_CHANNEL);
  while (!WiFi.STA.started()) {
    delay(100);
  }

  Serial.println("🚪 ESP32 Motion Sensor - Broadcast Master");
  Serial.println("Wi-Fi MAC: " + WiFi.macAddress());
  Serial.printf("Channel: %d\n", ESPNOW_WIFI_CHANNEL);

  if (!broadcast_peer.begin()) {
    Serial.println("ESP-NOW initialization failed. Rebooting...");
    delay(1000);
    ESP.restart();
  }

  Serial.println("Setup complete. Monitoring motion...");
}

void loop() {
  bool motion = digitalRead(SENSOR_PIN);
  unsigned long now = millis();

  if (motion == HIGH) {
    if (!motionDetected) {
      Serial.println("🚶 Motion detected: Someone crossed the doorway!");
      digitalWrite(INDICATOR_PIN, HIGH);
      broadcast_peer.send_message("Motion detected");
      motionDetected = true;
    }
    lastMotionTime = now;
  }

  if (motionDetected && motion == LOW && (now - lastMotionTime > CLEAR_DELAY)) {
    Serial.println("✅ Area is now clear.");
    digitalWrite(INDICATOR_PIN, LOW);
    motionDetected = false;
  }

  delay(50); // debounce / stability
}
