
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include "MPU9250.h"

#define PIN 6
#define NUM_LEDS 8
#define BRIGHTNESS 50

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

// an MPU9250 object with the MPU-9250 sensor on I2C
MPU9250FIFO IMU(Wire,0x68);

int r = 0;
int g = 0;
int b = 0;
uint32_t wheelVal = 0x00FFFFFF;

// variables to hold FIFO data, these need to be large enough to hold the data
float ax[100], ay[100], az[100];
size_t fifoSize;

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
  delay(wait);
}

/*
void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  if (topic.equals(pingCommandTopic)) {
    Serial.println("Sending ping...");
    Serial.println("Pinged!"); 
    // Start rotating
    // wait 5 seconds for connection:
    delay(5000);
  }
  else if (topic.equals(String("/arduino/rgb")))
  {
    Serial.println("RGB data received...");

    //r = payload.toInt();
    char buf[128];
    payload.toCharArray(buf,128);
    int n = sscanf(buf, "%d %d %d", &r, &g, &b);

    wheelVal= (r << 16) | (g << 8) | (b);
    //colorWipe(strip.Color( r,  g, b), 0); // Red
  }
  else
  {
    Serial.println(String("Unknown topic") + topic);
  }
}
*/


void setup() {
  Serial.begin(115200);

  int imu_status = IMU.begin();
  if (imu_status < 0) {
    Serial.println("IMU initialization unsuccessful");
    Serial.println("Check IMU wiring or try cycling power");
    Serial.print("Status: ");
    Serial.println(imu_status);
  }
  else {
      // setting the accelerometer full scale range to +/-8G 
      IMU.setAccelRange(MPU9250::ACCEL_RANGE_8G);
      // setting the gyroscope full scale range to +/-500 deg/s
      IMU.setGyroRange(MPU9250::GYRO_RANGE_500DPS);
      // setting DLPF bandwidth to 20 Hz
      IMU.setDlpfBandwidth(MPU9250::DLPF_BANDWIDTH_184HZ);
      // setting SRD to 19 for a 50 Hz update rate
      // Data Output Rate = 1000 / (1 + SRD)
      //1KHz
      IMU.setSrd(0);

      // enabling the FIFO to record just the accelerometers
      IMU.enableFifo(true,false,false,false);
    
    Serial.println("IMU initialized");
  }
  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

}

uint8_t red(uint32_t c) {
  return (c >> 16);
}
uint8_t green(uint32_t c) {
  return (c >> 8);
}
uint8_t blue(uint32_t c) {
  return (c);
}

void loop() {

  
  IMU.readFifo();
  // get the X, Y, and Z accelerometer data and their size
  IMU.getFifoAccelX_mss(&fifoSize,ax);
  IMU.getFifoAccelY_mss(&fifoSize,ay);
  IMU.getFifoAccelZ_mss(&fifoSize,az);
  //Serial.print("FIFO Size:"); Serial.println(fifoSize);

  //Read all fifo info in m/s^2
  for (int i = 0; i < fifoSize; i++) {
    //Acceleration magnitude
    float acc_val = sqrt(ax[i] *ax[i] + ay[i] * ay[i] + az[i] * az[i]);


    if (acc_val > 15.0) {
      //Init colors
      r = min(255, (int) (red(wheelVal) * acc_val / 10.0));
      g = min(255, (int) (green(wheelVal) * acc_val / 10.0));
      b = min(255, (int) (blue(wheelVal) * acc_val / 10.0));
      break;
    }
    else {
      //Serial.println(".");
      //Fading color
      r = max(0,r - 1);
      g = max(0,g - 1);
      b = max(0,b - 1);
    }
  }

  // Set Color
  colorWipe(strip.Color( r,  g, b), 0); // Red

  if (Serial.available())
  {
    int rv = 0;
    int gv = 0;
    int bv = 0;
    String val = Serial.readStringUntil('\n');
    int n = sscanf(val.c_str(), "%d %d %d", &rv, &gv, &bv);

    uint32_t wheel = ((rv & 0xFF) << 16) | ((gv & 0xFF) << 8) | (bv & 0xFF);
    
    if (wheel != wheelVal)
    {
      wheelVal = wheel;
      r = min(255, (int) (red(wheelVal)));
      g = min(255, (int) (green(wheelVal)));
      b = min(255, (int) (blue(wheelVal)));
      Serial.println("RGB Value updated.");
    }
    
  }
  
  delay(15);
}
