#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

#define BUTTON_PIN D3
#define OFF HIGH
#define ON LOW

uint8_t RECEVIER_MAC[] = {0x58, 0xbf, 0x25, 0xdb, 0x10, 0x11};
char msg[] = "Hello World!";
int button_state = OFF;

void on_sent(uint8_t *mac_addr, uint8_t sendStatus) {
    Serial.print("\r\nDelivery Status: ");
    Serial.println(sendStatus == 0 ? "Delivered Successfully" : "Delivery Fail");
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);

    delay(500);
    digitalWrite(LED_BUILTIN, LOW);

    Serial.begin(9600);
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != OK) {
        Serial.println("ESP-NOW init failed!");
        return;
    }
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

    esp_now_register_send_cb(on_sent);

    esp_now_add_peer(RECEVIER_MAC, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void loop() {
    int new_state = digitalRead(BUTTON_PIN);

    if (new_state != button_state) {
        Serial.print("Changed: ");
        Serial.println(new_state == ON ? "ON" : "OFF");
        digitalWrite(LED_BUILTIN, ON ? HIGH : LOW);

        if (new_state == ON) {
            esp_now_send(RECEVIER_MAC, (uint8_t*)msg, sizeof(msg));
        }
    }

    delay(50);
    button_state = new_state;
}
