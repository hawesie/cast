#include "DirectAccessWriter.hpp"

using namespace cast;
using namespace cast::cdl;

extern "C" {
  cast::CASTComponentPtr newComponent() {
    return new DirectAccessWriter();
  }
}


void DirectAccessWriter::runComponent() {
  
  //switch behaviour between check and alter
  bool check = false;
  
  // Create a struct with a known value
  int startingValue = 0;
  TestStructIntPtr tsi = new TestStructInt(startingValue);
  
  // and add it to WM
  addToWorkingMemory(newDataID(), tsi);
  
  while (isRunning()) {
    
    if(check) {
      // If the value has changed, kick up a fuss
      if (tsi->dummy != startingValue) {
        throw(CASTException("Value has changed!"));
      }
      else {
        println("fine");
      }
    }
    else {
      tsi->dummy++;
    }
    sleepComponent(1000);
  }
  
}