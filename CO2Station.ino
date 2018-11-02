// Thanks to https://github.com/GeorgK/MQ135.git
// for the MQ135 sensor library. I burned the sensor for
// 24 hours first, corrected the RZero value and updated
// RZero value in the header file.
#include "MQ135.h"
#include "LiquidCrystal.h"
#include "LcdBarGraph.h"
#include <dht.h>

enum DisplayState {
  Text, // Regular display.
  Graph, // Button press, breath monitor.
  WarmUp
};

// MQ135 sensor.  
const int sensorInput = A2; 

// DHT11 sensor to record correct temperate and humidity. 
// This will ensure we get accurate readings.
const int DHT_PIN = 2; // Digital read pin.
float temperature = 25; // assume current temperature. Recommended to measure with DHT22
float humidity = 5; // assume current humidity. Recommended to measure with DHT22
dht DHT;

// Define the gas sensor. 
MQ135 gasSensor = MQ135(sensorInput);
float prevCo2Val = -1.0;
float mq135Smoothed = 0; 
float mq135Correction = 0.80;

// Initialize the LCD.
LiquidCrystal lcd(7, 8, 9, 10, 11 , 12);
boolean updateLCD = false;

// Bar graph initiation. We will use 2 of them
// to represent the amount of breath. 
byte lcdNumCols = 16;
LcdBarGraph lbg0(&lcd, lcdNumCols, 0, 0);
LcdBarGraph lbg1(&lcd, lcdNumCols, 0, 1);

// LCD Display default state.
DisplayState lcdState = Text;
unsigned long startTime;

// Button
const int captureButton = 4;
boolean captureButtonState;

// LEDs. 
const int ledDisengaged = 3;
const int ledEngaged = 5;

// Stabilize time to spend in the WarmUp state. 
const int stabilizeTime = 5; // seconds

void setup() {
  Serial.begin(9600);
  
  // Setup the CO2 sensor input pin. 
  pinMode(sensorInput, INPUT);
  pinMode(captureButton, INPUT);
  pinMode(ledDisengaged, OUTPUT);
  pinMode(ledEngaged, OUTPUT);
 
  // Set LCD max bounds.
  lcd.begin(16, 2);
  setInitialLCDDisplay();

  // Initial delay to get the DHT sensor ready. 
  delay(2000);

  // Update temperate and humidity values. 
  // readFromDht();
}

void loop() {
  // Read the button state. 
  captureButtonState = digitalRead(captureButton);

  // MQ135 correction val 
  float mq135Val = gasSensor.getCorrectedPPM(temperature, humidity);
  mq135Smoothed = mq135Correction * mq135Smoothed + (1 - mq135Correction) * mq135Val;

  // Define Text state. 
  if (lcdState == Text) {
    // Button is pressed. 
    if (captureButtonState == HIGH) {
       // Update LEDs.
      analogWrite(ledDisengaged, 0);
      analogWrite(ledEngaged, 255);

      // Clear the LCD.
      lcd.clear();
      
      // Update state since button is pressed. 
      lcdState = Graph;
    }
    
    if (captureButtonState == LOW) {
      // Update the LEDs.
      analogWrite(ledEngaged, 0);
      analogWrite(ledDisengaged, 255);
    }
  } 

  // Define Graph state. 
  if (lcdState == Graph) {
    // Print the graph on LCD.
    printBreathGraphOnLCD();

    // Button is pressed. 
    if (captureButtonState == HIGH) {
      // Update LEDS. 
      analogWrite(ledEngaged, 255);
      analogWrite(ledDisengaged, 0);
    }

    // Button is released. 
    if (captureButtonState == LOW) {
      lcd.clear();
      printLCDFirstRow("Look back..");
      
      // Update second row of LCD. 
      printLCDSecondRow("Ready in ");

      // Update the LEDs.
      analogWrite(ledEngaged, 0);
      analogWrite(ledDisengaged, 255);

      // Reset prevCo2Val
      prevCo2Val = -1;

      // Update state.
      lcdState = WarmUp;

      // Reset startTime to current time to initiate warm up.
      startTime = millis();
    }
  }

  // Warm up state.
  if (lcdState == WarmUp) {
    unsigned long endTime = millis();

    // Capture button state will always be 0 till the warm up completes.
    captureButtonState = 0;

    // Calculate elapsedTime in millis. 
    float elapsedTimeInMillis = endTime - startTime;
    
    // Update time in second row. 
    updateTimeInSecondRow(elapsedTimeInMillis);
    
    if (elapsedTimeInMillis > stabilizeTime * 1000) {
      // 9 second warm up time. 
      lcdState = Text;
      
      // Reset sensor by resetting Rzero.
      float correctedRZero = gasSensor.getCorrectedRZero(temperature, humidity);
      
      setInitialLCDDisplay();
    }
  }

  //Debug MQ-135 sensor value.
  //printCO2Debug();

  // Send serial data from CO2 station. 
  sendSerialData();
}

void readFromDht() {
  int chk = DHT.read11(DHT_PIN);

  temperature = DHT.temperature; 
  humidity = DHT.humidity;
  // DISPLAY DATA
  Serial.print(DHT.humidity, 1);
  Serial.print(",\t");
  Serial.println(DHT.temperature, 1);
}

void updateTimeInSecondRow(int t) {
  lcd.setCursor(9, 1);
  lcd.print(stabilizeTime - t/1000);
}

void setInitialLCDDisplay() {
  lcd.clear();
  printLCDFirstRow("Press...Exhale..");
  printLCDSecondRow("Into this hole->");
}

void printLCDSecondRow(String text) {
  // 2nd row, 1st column.
  lcd.setCursor(0, 1);
  lcd.print(text);
}

void printLCDFirstRow(String text) {
  lcd.setCursor(0, 0); 
  lcd.print(text);
}

void printBreathGraphOnLCD() {
  float diff;
  if (prevCo2Val == -1) {
    prevCo2Val = mq135Smoothed;
  }

  // Get new CO2Val and calculate the difference between the two.
  float newCo2Val = mq135Smoothed;
  diff = newCo2Val - prevCo2Val;

  // Don't draw if diff is not > 0. We get weird characters on screen.
  if (diff > 0) {
    // Draw the bar. 
    lbg0.drawValue(diff, 80);
    lbg1.drawValue(diff, 80); 
  }
  delay (100);
}

void sendSerialData() {
  Serial.print(captureButtonState);
  Serial.print(",");
  Serial.print((int) mq135Smoothed);
  Serial.print("\n");
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
