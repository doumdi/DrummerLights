#include <Arduino.h>

#include <defines.h>
#include <ioexpander.h>
#include <SPI.h>
#include "Wire.h"
#include <Adafruit_NeoPixel.h>

IOExpander ioExpander;
TaskHandle_t ledBlinkHandle = NULL;

#define PIN 17
#define NUM_LEDS 8
#define BRIGHTNESS 50

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

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

  //BATT READ ENABLE
  ioExpander.pinMode(EXT_PIN13_BATT_READ_EN, OUTPUT);
  ioExpander.digitalWrite(EXT_PIN13_BATT_READ_EN, HIGH);

  //Enable External Power, Low State to pilot the transistor
  ioExpander.pinMode(EXT_PIN14_EXTERNAL_POWER_EN, OUTPUT);
  ioExpander.digitalWrite(EXT_PIN14_EXTERNAL_POWER_EN,LOW);

  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Initialized...");

  //On Arduino Core, blink led
  xTaskCreatePinnedToCore(&ledBlink, "Blinky", 2048, NULL, 1, &ledBlinkHandle,1);
  
  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

int r = 255;
int g = 0;
int b = 0;


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
  delay(wait);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("sleeping..");
  delay(1000);
  colorWipe(strip.Color( r,  g, b), 0); // Red
}