
#include <Wire.h>// This library allows you to communicate with I2C devices.
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp; // I2C
const int MPU_ADDR = 0x68; // I2C address
int16_t accelerometer_x, accelerometer_y, accelerometer_z; // variables for accelerometer raw data
int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
int16_t temperature; // variables for temperature data

int totaal = 0;
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


void setup() {
  pinMode(23,OUTPUT);
  digitalWrite(23,HIGH);
  
  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    while (1);
  }
  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }

  Wire.begin();
  Wire.beginTransmission(MPU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

void loop() {
  for (int i = 0; i < 50; i++) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
    Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
    Wire.requestFrom(MPU_ADDR, 7 * 2, true); // request a total of 7*2=14 registers
    // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
    accelerometer_y = Wire.read() << 8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
    totaal = totaal + accelerometer_y;
    Serial.print("Y= ");
    Serial.println(accelerometer_y);
    Serial.print("Totaal= ");
    Serial.println(totaal);
  }
 
  float gem = totaal / 50;
  Serial.print("Gem: ");
  Serial.println(gem);
  float graden = (gem / 16384) * 90;
   

  Serial.print(F("Temperature = "));
  Serial.print(bmp.readTemperature());
  Serial.println(" | aY = "); Serial.print(accelerometer_y);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.print("y = ");
  display.print(graden);
  display.print(" graden");
  display.setCursor(0,20);
  display.print("Temp= ");
  display.print(bmp.readTemperature());
  display.print(" C");
  display.display();
  totaal = 0; //reset total to calculate average again

}
