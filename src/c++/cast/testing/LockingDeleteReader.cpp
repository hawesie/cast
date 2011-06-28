#include "LockingDeleteReader.hpp"

using namespace cast;
using namespace cast::cdl;

extern "C" {
  cast::CASTComponentPtr newComponent() {
    return new LockingDeleteReader();
  }
}


void LockingDeleteReader::start() {
  addChangeFilter(createLocalTypeFilter<TestStructInt>(cdl::ADD),
                  new MemberFunctionChangeReceiver<LockingDeleteReader>( this,
                                                                       &LockingDeleteReader::testStructAdded));  
}

void LockingDeleteReader::testStructAdded(const cast::cdl::WorkingMemoryChange & _wmc) {

  //Delay a little to ensure lock has happened
  sleepComponent(1000);

  println("trying to read");
  try {
    TestStructIntPtr tsi = getMemoryEntry<TestStructInt>(_wmc.address);
  println("read");    
  }
  catch(const DoesNotExistOnWMException &e) {
    error(e.what());
  }

  
}