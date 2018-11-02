#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofBackground(0);
  
    // Arduino to talk to.
    serial.setup("/dev/cu.usbmodem1411", 9600);
    // Setup Osc.
    oscHandler.setup();
}

//--------------------------------------------------------------
void ofApp::update(){
    // As long as there is serial data available to read, repeat.
    while (serial.available() > 0)
    {
        // [Summary] Data is sent from the Arduino byte by byte.
        // In this while loop, we read the data byte by byte until
        // there is incoming data. Every character we are sending from
        // there is a byte wide. We keep constructing the buffer until
        // we reach the new line character, which determines that we are
        // done reading the first set of data.
    
        // Read the byte.
        char b = serial.readByte();
        
        // End of line character.
        if (b == '\n')
        {
            // Split the buffer on the commas.
            std::vector<std::string> tokens = ofSplitString(buffer, ",");
          
            // The number of tokens in our packet is 2, here we check to make
            // sure that our packet is correctly formed.
            if (tokens.size() == 2)
            {
                // Capacitive touch dish Left.
                captureButtonState = ofToInt(tokens[0]);
                sensorVal = ofToInt(tokens[1]);
              
                // Make OSC Message
                ofxOscMessage m;
                m.setAddress(string(SENDMESSAGE));
                m.addIntArg(captureButtonState);
                m.addIntArg(sensorVal);
            
                // Send capture button state and sensor val.
                oscHandler.sendOSCMessageTree(m);
                oscHandler.sendOSCMessageProjection(m);
            }
        
            std::cout << ofToString(tokens) << std::endl;
            
            // Empty the buffer.
            buffer = "";
        }
        else
        {
            // If it's not the line feed character, add it to the buffer.
            buffer += b;
        }
    }
}

void ofApp::draw(){
}

void ofApp::exit(){
}

