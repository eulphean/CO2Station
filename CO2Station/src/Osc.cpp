#include "Osc.h"

void Osc::setup() {
  // Setup the sender for both the machines. 
  senderProjection.setup(HOST1, PORTTOSEND1);
  senderTree.setup(HOST2, PORTTOSEND2);
}

void Osc::sendOSCMessageProjection(ofxOscMessage &m) {
  senderProjection.sendMessage(m, false);
}

// Route these messages directly to Mac mini.
void Osc::sendOSCMessageTree(ofxOscMessage &m) {
  senderTree.sendMessage(m, false);
}
