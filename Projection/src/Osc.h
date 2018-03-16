#pragma once
#include "ofMain.h"
#include "ofxOsc.h"

#define PORT 12345

class Osc {
  public:
    void setup();
    void update();
    void processOscMessage(ofxOscMessage m);
    
  private:
    // To send OSC messages to another machine.
    ofxOscReceiver receiver;
};
