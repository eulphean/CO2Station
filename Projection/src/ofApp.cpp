// Setup projection mapping with this.

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
  ofBackground(0);

  // Setup Osc.
  oscHandler.setup();
}

//--------------------------------------------------------------
void ofApp::update(){
  oscHandler.update();
}

void ofApp::draw(){

}

