#include "DirectAccessReader.hpp"

using namespace cast;
using namespace cast::cdl;

extern "C" {
  cast::CASTComponentPtr newComponent() {
    return new DirectAccessReader();
  }
}


void DirectAccessReader::start() {
  addChangeFilter(createLocalTypeFilter<TestStructInt>(cdl::ADD),
                  new MemberFunctionChangeReceiver<DirectAccessReader>( this,
                                                                       &DirectAccessReader::testStructAdded));  
}

void DirectAccessReader::testStructAdded(const cast::cdl::WorkingMemoryChange & _wmc) {
  TestStructIntPtr tsi = getMemoryEntry<TestStructInt>(_wmc.address);
  
  //switch behaviour between check and alter
  bool alter = false;
  
  if(alter) {
    tsi->dummy++;
  }
  else {
    int startingValue = tsi->dummy;
    
    while(isRunning()) {
      tsi = getMemoryEntry<TestStructInt>(_wmc.address);
      if (tsi->dummy != startingValue) {
        throw(CASTException("Value has changed!"));
      }
      else {
        println("fine");
      }
      sleepComponent(1000);
    }
    
  }
}