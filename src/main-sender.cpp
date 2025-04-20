#include <Arduino.h>
#include "DFRobot_GDL.h"
#include <ESP8266WiFi.h>
#include <espnow.h>

#define BUTTON_PIN D3
#define OFF HIGH
#define ON LOW
#define TFT_DC D2
#define TFT_CS D8
#define TFT_RST D1

DFRobot_ST7735_80x160_HW_SPI screen(TFT_DC, TFT_CS, TFT_RST);
uint8_t RECEIVER_MAC[] = {0x58, 0xbf, 0x25, 0xdb, 0x10, 0x11};
const char MSG[] = "Ding dong";

struct sender_state_t {
    int button = OFF;
    bool did_send = false;
    bool send_result = false;
};

sender_state_t state;

void resetScreen() {
    screen.setTextWrap(false);
    screen.fillScreen(COLOR_RGB565_BLACK);

    screen.setCursor(0, 0);
    screen.setTextColor(0xFFFF);
}

void printWelcome() {
    resetScreen();
    screen.setTextSize(3);
    screen.println("Welcome!");
    screen.setTextSize(2);
    screen.println("Dethy + Lucier");
}

void printResult(bool success) {
    resetScreen();
    screen.setTextSize(2);

    if (success){
        screen.println("Ding dong...");
    }
    else {
        screen.setTextColor(COLOR_RGB565_RED);
        screen.println("Failed!");
        screen.println("Please knock...");
    }

    delay(1000);
    printWelcome();
}

void setupScreen() {
    screen.begin();
    printWelcome();
}

void on_sent(uint8_t *mac_addr, uint8_t sendStatus) {
    Serial.print("\r\nDelivery Status: ");
    Serial.println(sendStatus == 0 ? "Delivered Successfully" : "Delivery Fail");

    state.did_send = true;
    state.send_result = sendStatus == 0;
}

// MAIN

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

    esp_now_add_peer(RECEIVER_MAC, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
    digitalWrite(LED_BUILTIN, HIGH);

    setupScreen();
}

void loop() {
    int new_state = digitalRead(BUTTON_PIN);

    if (new_state != state.button) {
        Serial.print("Changed: ");
        Serial.println(new_state == ON ? "ON" : "OFF");
        digitalWrite(LED_BUILTIN, new_state ? HIGH : LOW);

        if (new_state == ON) {
            esp_now_send(RECEIVER_MAC, (uint8_t*)MSG, sizeof(MSG));
        }
    }
    state.button = new_state;

    // handle screen
    if (state.did_send) {
        printResult(state.send_result);
        state.did_send = false;
    }

    delay(50);
}
