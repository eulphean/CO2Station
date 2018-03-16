#include "Osc.h"

void Osc::setup() {
  // Setup receiving.
  receiver.setup(PORT);
}

void Osc::update() {
  while (receiver.hasWaitingMessages()) {
    ofxOscMessage m;
    // Set the next message.
    #pragma warning(disable: WARNING_CODE)
    
    receiver.getNextMessage(&m);
    
    // Process incoming Wekinator commands.
    processOscMessage(m);
  }
}

void Osc::processOscMessage(ofxOscMessage m) {
  if (m.getAddress() == "/co2/sensor") {
    int sensorVal = m.getArgAsInt(0);
    cout << sensorVal << endl;
    // Now, we can use this sensor value to do whatever munging we want to.
  }
}
