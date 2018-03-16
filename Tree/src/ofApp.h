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
  
    // OSC
    Osc oscHandler;
};
