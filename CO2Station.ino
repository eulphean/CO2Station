// Thanks to https://github.com/GeorgK/MQ135.git
// for the MQ135 sensor library. I burned the sensor for
// 24 hours first, corrected the RZero value and updated
// RZero value in the header file.
#include "MQ135.h"
#include "LiquidCrystal.h"

// MQ135 sensor.  
const int sensorInput = A2; 
float temperature = 21.0; // assume current temperature. Recommended to measure with DHT22
float humidity = 25.0; // assume current humidity. Recommended to measure with DHT22

// Define the gas sensor. 
MQ135 gasSensor = MQ135(sensorInput);

// Initialize the LCD.
LiquidCrystal lcd(7, 8, 9, 10, 11 , 12);
boolean updateLCD = false;

// Button
const int captureButton = 4; 
boolean captureState;

// LEDs. 
const int ledDisengaged = 3;
const int ledEngaged = 5; 

void setup() {
  Serial.begin(9600);
  
  // Setup the CO2 sensor input pin. 
  pinMode(sensorInput, INPUT);
  pinMode(captureButton, INPUT);
  pinMode(ledDisengaged, OUTPUT);
  pinMode(ledEngaged, OUTPUT);

  // Initial display of the LCD. 
  setInitialLCDDisplay();
}

void loop() {
  // Read the button state. 
  captureState = digitalRead(captureButton); 
  
  updateCO2LCD(getCO2ppm());
  // Debug LCD print. 
  printCO2Debug();

  if (captureState == HIGH) {
    // Update LEDs.
    analogWrite(ledDisengaged, 0);
    analogWrite(ledEngaged, 255);
  } else {
    // Update LEDs.
    analogWrite(ledEngaged, 0);
    analogWrite(ledDisengaged, 255);
  }
}

float getCO2ppm() {
  float ppm = gasSensor.getCorrectedPPM(temperature, humidity);
  return ppm;
}

void setInitialLCDDisplay() {
  lcd.clear();
  lcd.setCursor(0, 1); 
  lcd.print("CO2: ");
}

void updateCO2LCD(float val) {
  lcd.setCursor(4, 1);
  lcd.print((int)val);
  lcd.print("ppm");
}

void printCO2Debug() {
  float rzero = gasSensor.getRZero();
  float correctedRZero = gasSensor.getCorrectedRZero(temperature, humidity);
  float resistance = gasSensor.getResistance();
  float ppm = gasSensor.getPPM();
  float correctedPPM = gasSensor.getCorrectedPPM(temperature, humidity);
  
  Serial.print("MQ135 RZero: ");
  Serial.print(rzero);
  Serial.print("\t Corrected RZero: ");
  Serial.print(correctedRZero);
  Serial.print("\t Resistance: ");
  Serial.print(resistance);
  Serial.print("\t PPM: ");
  Serial.print(ppm);
  Serial.print("\t Corrected PPM: ");
  Serial.print(correctedPPM);
  Serial.println("ppm");
}

