// Thanks to https://github.com/GeorgK/MQ135.git
// for the MQ135 sensor library. I burned the sensor for
// 24 hours first, corrected the RZero value and updated
// RZero value in the header file.
#include "MQ135.h"
#include "LiquidCrystal.h"
#include "LcdBarGraph.h"

enum DisplayState {
  Text, // Regular display.
  Graph, // Button press, breath monitor.
  WarmUp
};

// FSR sensor. 
int fsrPin = A0; 
int fsrReading;
float smoothFsr = 0;
float correction = 0.75;

// MQ135 sensor.  
const int sensorInput = A2; 
float temperature = 21.0; // assume current temperature. Recommended to measure with DHT22
float humidity = 25.0; // assume current humidity. Recommended to measure with DHT22

// Define the gas sensor. 
MQ135 gasSensor = MQ135(sensorInput);
float prevCo2Val = -1.0;

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
}

void loop() {
  // Read the button state. 
  //captureButtonState = digitalRead(captureButton);

  // Read the FSR
  fsrReading = analogRead(fsrPin);
  smoothFsr = correction * smoothFsr + (1 - correction) * fsrReading;
  captureButtonState = (smoothFsr > 100);

  // Define text state. 
  if (lcdState == Text) {
    // Update LCD with CO2 measurements. 
    updateCO2LCD(gasSensor.getCorrectedPPM(temperature, humidity));

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
  } 

  // Define graph state. 
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
      setInitialLCDDisplay();

      // Update the LEDs.
      analogWrite(ledEngaged, 0);
      analogWrite(ledDisengaged, 255);

      // Reset prevCo2Val
      prevCo2Val = -1;

      // Update state.
      lcdState = WarmUp;

      // Reset startTime to current time to initiate warm up.
      startTime = millis();

      // Update second row of LCD. 
      printLCDSecondRow("Warming up...");
    }
  }

  if (lcdState == WarmUp) {
    unsigned long endTime = millis();
    
    updateCO2LCD(gasSensor.getCorrectedPPM(temperature, humidity));
    
    if (endTime - startTime > 10 * 1000) {
      // 10 second warm up time. 
      lcdState = Text;

      // Update second row of LCD. 
      printLCDSecondRow("Push...Exhale");
    }
  }

  // Debug MQ-135 sensor value.
  //printCO2Debug();

  // Send serial data from CO2 station. 
  sendSerialData();
}

void setInitialLCDDisplay() {
  // 1st row, 1st column.
  lcd.clear();
  lcd.setCursor(0, 0); 
  lcd.print("CO2: ");
  printLCDSecondRow("Push...Exhale");
}

void printLCDSecondRow(String text) {
  // 2nd row, 1st column.
  lcd.setCursor(0, 1);
  lcd.print(text);
}

void updateCO2LCD(float val) {
  // 1st row, 4th column.
  lcd.setCursor(4, 0);
  lcd.print((int)val);
  lcd.print("ppm");
}

void printBreathGraphOnLCD() {
  float diff;
  if (prevCo2Val == -1) {
    prevCo2Val = gasSensor.getCorrectedPPM(temperature, humidity);
  }

  // Get new CO2Val and calculate the difference between the two.
  float newCo2Val = gasSensor.getCorrectedPPM(temperature, humidity);
  diff = newCo2Val - prevCo2Val;

  // Don't draw if diff is not > 0. We get weird characters on screen.
  if (diff > 0) {
    // Draw the bar. 
    lbg0.drawValue(diff, 70);
    lbg1.drawValue(diff, 70); 
  }
  
  delay (300);
}

void sendSerialData() {
  Serial.print(captureButtonState);
  Serial.print(",");
  Serial.print((int)gasSensor.getCorrectedPPM(temperature, humidity));
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

