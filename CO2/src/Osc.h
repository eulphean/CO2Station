#pragma once
#include "ofMain.h"
#include "ofxOsc.h"

class Osc {
  public:
    void setup();
    void sendOSCMessageProjection(ofxOscMessage &m);
    void sendOSCMessageTree(ofxOscMessage &m);
    
  private:
    // To send OSC messages to another machine.
    ofxOscSender senderProjection;
    ofxOscSender senderTree;
};
