#pragma once
#include "ofMain.h"
#include "ofxOsc.h"

#define HOST1 "10.73.38.86" // Update this IP address to the IP of the other MAC.
#define PORTTOSEND1 12345

#define HOST2 "10.73.38.86" // Update this IP address to the IP of the other MAC.
#define PORTTOSEND2 12346

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
