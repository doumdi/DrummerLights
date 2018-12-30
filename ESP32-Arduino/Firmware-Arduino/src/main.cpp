#include <Arduino.h>

#include <defines.h>
#include <ioexpander.h>
#include <SPI.h>
#include "Wire.h"

IOExpander ioExpander;
TaskHandle_t ledBlinkHandle = NULL;

void ledBlink(void *pvParameters)
{
  while(1)
  {
    ioExpander.digitalWrite(EXT_PIN01_LED, LOW);
    delay(500);
    ioExpander.digitalWrite(EXT_PIN01_LED, HIGH);
    delay(500);
  }
}

void setup() {

  // Setup I2C PINS, SDA, SCL, Clock speed
  //Wire.begin(23, 25, 400000);
  //EXP Chip Select
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);

  //SCLK
  pinMode(19, OUTPUT);
  //MOSI
  pinMode(18, OUTPUT);
  //MISO
  pinMode(39, INPUT);
  //nINT
  pinMode(36, INPUT_PULLUP);

  //SCK, MISO, MOSI no SS pin
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  SPI.begin(19, 39, 18);

  ioExpander.begin();

  //ALIVE -->HIGH, power will stay on
  ioExpander.pinMode(EXT_PIN12_KEEP_ALIVE, OUTPUT);
  ioExpander.digitalWrite(EXT_PIN12_KEEP_ALIVE, HIGH);

  //LED
  ioExpander.pinMode(EXT_PIN01_LED, OUTPUT);
  ioExpander.digitalWrite(EXT_PIN01_LED, HIGH);

  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Initialized...");

  //On Arduino Core, blink led
  xTaskCreatePinnedToCore(&ledBlink, "Blinky", 2048, NULL, 1, &ledBlinkHandle,1);

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("sleeping..");
  delay(1000);
}