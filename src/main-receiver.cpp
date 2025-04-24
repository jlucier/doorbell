#include <Arduino.h>
#include <SerialMP3Player.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

SerialMP3Player mp3(RX, TX);

struct message_t {
    char data[32];
};

message_t msg;

void play_sound() {
    digitalWrite(LED_BUILTIN, LOW);
    mp3.play(1, 20);
    digitalWrite(LED_BUILTIN, HIGH);
}


void on_recv(uint8_t* mac, uint8_t* data, uint8_t len) {
    memcpy(&msg, data, sizeof(msg));
    Serial.println(msg.data);
    play_sound();
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    Serial.begin(9600);
    mp3.begin(9600);
    delay(1000);
    mp3.sendCommand(CMD_SEL_DEV, 0, 2);   //select sd-card
    delay(1000);
    play_sound();

    WiFi.mode(WIFI_STA);

    if (esp_now_init() != OK) {
        Serial.println("ESP-NOW init failed!");
        return;
    }

    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
    esp_now_register_recv_cb(on_recv);
    digitalWrite(LED_BUILTIN, HIGH);
}

#define ON LOW
#define OFF HIGH
bool button = OFF;

void loop() {
    int new_state = digitalRead(D3);

    if (new_state != button) {
        digitalWrite(LED_BUILTIN, new_state ? HIGH : LOW);

        if (new_state == ON) {
            play_sound();
        }
    }
    button = new_state;
}
