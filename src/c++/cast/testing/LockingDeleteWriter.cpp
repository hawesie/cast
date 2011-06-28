#include "LockingDeleteWriter.hpp"

using namespace cast;
using namespace cast::cdl;

extern "C" {
  cast::CASTComponentPtr newComponent() {
    return new LockingDeleteWriter();
  }
}


void LockingDeleteWriter::runComponent() {
  
  
  // Create a struct
  TestStructIntPtr tsi = new TestStructInt(0);
  
  const WorkingMemoryAddress wma = cast::makeWorkingMemoryAddress(newDataID(), getSubarchitectureID());

  // and add it to WM
  addToWorkingMemory(wma, tsi);
  println("added");
  
  
  lockEntry(wma, cdl::LOCKEDODR);
  println("locked");
  
  sleepComponent(5000);
  
  deleteFromWorkingMemory(wma);
  println("deleted");

  
}