#include <Arduino.h>
#include <SerialMP3Player.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

// #define TX 22
// #define RX 21

SerialMP3Player mp3(RX, TX);

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  mp3.begin(9600);
  delay(500);
  mp3.sendCommand(CMD_SEL_DEV, 0, 2);   //select sd-card
  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, HIGH);
  mp3.play(1, 10);
  delay(3000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}
