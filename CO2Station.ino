// Thanks to https://github.com/GeorgK/MQ135.git
// for the MQ135 sensor library. I burned the sensor for
// 24 hours first, corrected the RZero value and updated
// RZero value in the header file.
#include "MQ135.h"
#include "LiquidCrystal.h"
#include "LcdBarGraph.h"

enum DisplayState {
  Text, // Regular display.
  Graph // Button press, breath monitor.
};

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
 
  // Set LCD max bounds.
  lcd.begin(16, 2);
  setInitialLCDDisplay();
}

void loop() {
  // Read the button state. 
  captureState = digitalRead(captureButton); 

  // Define text state. 
  if (lcdState == Text) {
    // Update LCD with CO2 measurements. 
    updateCO2LCD(gasSensor.getCorrectedPPM(temperature, humidity));

    // Button is pressed. 
    if (captureState == HIGH) {
       // Update LEDs.
      analogWrite(ledDisengaged, 0);
      analogWrite(ledEngaged, 255);

      // Clear the LCD.
      lcd.clear();
      
      // Update state since button is pressed. 
      lcdState = Graph;
    }

    // Button is released. 
    if (captureState == LOW) {
       analogWrite(ledEngaged, 0);
       analogWrite(ledDisengaged, 255);
    }
  } 

  // Define graph state. 
  if (lcdState == Graph) {
    // Print the graph on LCD.
    printBreathGraphOnLCD();

    // Button is pressed. 
    if (captureState == HIGH) {
      // Update LEDS. 
      analogWrite(ledEngaged, 255);
      analogWrite(ledDisengaged, 0);
    }

    // Button is released. 
    if (captureState == LOW) {
      setInitialLCDDisplay();

      // Reset prevCo2Val
      prevCo2Val = -1;
      
      // Update state.
      lcdState = Text;
    }
  }
  
  // Debug LCD print. 
  printCO2Debug();
}

void setInitialLCDDisplay() {
  // 1st row, 1st column.
  lcd.clear();
  lcd.setCursor(0, 0); 
  lcd.print("CO2: ");
  printLCDSecondRow();
}

void printLCDSecondRow() {
  // 2nd row, 1st column.
  lcd.setCursor(0, 1);
  lcd.print("Push...Exhale");
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
    lbg0.drawValue(diff, 100);
    lbg1.drawValue(diff, 100); 
  }

  Serial.print("Prev Value: ");
  Serial.print(prevCo2Val);
  Serial.print("\t New Val: ");  
  Serial.print(newCo2Val);
  Serial.print("\n");

  delay (100);
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

