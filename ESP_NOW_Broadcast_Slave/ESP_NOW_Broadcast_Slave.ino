#include "ESP32_NOW.h"
#include "WiFi.h"
#include <esp_mac.h>
#include <vector>

#define ESPNOW_WIFI_CHANNEL 6
#define INDICATOR_PIN 15
#define LIGHT_ON_TIME 3000  // milliseconds

// === Motion state flags ===
bool motionReceived = false;
unsigned long lastMotionTime = 0;

class ESP_NOW_Peer_Class : public ESP_NOW_Peer {
public:
  ESP_NOW_Peer_Class(const uint8_t *mac_addr, uint8_t channel, wifi_interface_t iface, const uint8_t *lmk)
    : ESP_NOW_Peer(mac_addr, channel, iface, lmk) {}

  ~ESP_NOW_Peer_Class() {}

  bool add_peer() {
    if (!add()) {
      log_e("Failed to register the broadcast peer");
      return false;
    }
    return true;
  }

  void onReceive(const uint8_t *data, size_t len, bool broadcast) {
    Serial.printf("Received a message from master " MACSTR " (%s)\n", MAC2STR(addr()), broadcast ? "broadcast" : "unicast");
    Serial.printf("  Message: %s\n", (char *)data);

    if (strcmp((char *)data, "Motion detected") == 0) {
      Serial.println("🔔 Motion event received!");
      Serial.println("[MOTION]"); // For Node.js trigger
      motionReceived = true;
      lastMotionTime = millis();
    }
  }
};

std::vector<ESP_NOW_Peer_Class> masters;

// Callback for unknown master registration
void register_new_master(const esp_now_recv_info_t *info, const uint8_t *data, int len, void *arg) {
  if (memcmp(info->des_addr, ESP_NOW.BROADCAST_ADDR, 6) == 0) {
    Serial.printf("Unknown peer " MACSTR " sent a broadcast message\n", MAC2STR(info->src_addr));
    Serial.println("Registering as a master...");

    ESP_NOW_Peer_Class new_master(info->src_addr, ESPNOW_WIFI_CHANNEL, WIFI_IF_STA, NULL);
    masters.push_back(new_master);
    if (!masters.back().add_peer()) {
      Serial.println("Failed to register new master");
    }
  } else {
    log_v("Unicast message from " MACSTR ", ignored", MAC2STR(info->src_addr));
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(INDICATOR_PIN, OUTPUT);
  digitalWrite(INDICATOR_PIN, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.setChannel(ESPNOW_WIFI_CHANNEL);
  while (!WiFi.STA.started()) {
    delay(100);
  }

  Serial.println("ESP-NOW Broadcast Slave with Indicator");
  Serial.println("MAC Address: " + WiFi.macAddress());

  if (!ESP_NOW.begin()) {
    Serial.println("ESP-NOW init failed. Rebooting...");
    delay(1000);
    ESP.restart();
  }

  ESP_NOW.onNewPeer(register_new_master, NULL);
  Serial.println("Setup done. Waiting for broadcasts...");
}

void loop() {
  if (motionReceived) {
    digitalWrite(INDICATOR_PIN, HIGH);

    if (millis() - lastMotionTime > LIGHT_ON_TIME) {
      digitalWrite(INDICATOR_PIN, LOW);
      motionReceived = false;
      Serial.println("✅ Indicator off.");
    }
  }

  delay(50);
}
