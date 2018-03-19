#pragma once

#include "ofMain.h"
#include "Osc.h"

using namespace std;
#define SENDMESSAGE "/co2/sensor"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    void exit();

    // An instance of the serial object. One instance is needed for each active
    // serial connection.
    ofSerial serial;

    // A buffer to store our buffered data.
    string buffer;
  
    // Arduino data.
    unsigned int captureButtonState;
    unsigned int sensorVal;
//    int maxSensorVal = -9999;
//    int minSensorVal = 9999;
  
    // OSC
    Osc oscHandler;
     
};
