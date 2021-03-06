/*
 * Comedian example code to demonstrate CAST functionality.
 *
 * Copyright (C) 2006-2007 Nick Hawes
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "FunnyMan.hpp"
#include <ComedyEssentials.hpp>

#include <ChangeFilterFactory.hpp>
using namespace comedyarch::autogen;

using namespace std;
using namespace boost;
using namespace cast;


/**
 * The function called to create a new instance of our component.
 *
 * Taken from zwork
 */
extern "C" {
  cast::CASTComponentPtr
  newComponent() {
    return new FunnyMan();
  }
}



void FunnyMan::configure(const std::map<std::string,std::string> & _config) {

  std::map<std::string,std::string>::const_iterator configIter 
    = _config.find("--punchline");
  
  if(configIter != _config.end()) {
    m_punchline = configIter->second;
  }

  log("punchline set: %s", m_punchline.c_str());
}


void FunnyMan::start() {
  
  //create a WorkingMemoryChangeReceiver that automatically calls a
  //member function of this class
  MemberFunctionChangeReceiver<FunnyMan> * pReceiver = 
    new MemberFunctionChangeReceiver<FunnyMan>(this,
					       &FunnyMan::newTwoLinerAdded);
    
  //add this receiver to listen for changes
  addChangeFilter(//listen for joke types
		  //that are added
		  //that are local to this subarchitecture
		  createLocalTypeFilter<TwoLiner>(cdl::ADD),
		  //the receiver object
		  pReceiver);  

//   //tests for matching type hierarchies

//   //add this receiver to listen for changes
//   addChangeFilter(//listen for joke types
// 		  //that are added
// 		  //that are local to this subarchitecture
// 		  createLocalTypeFilter<OneLiner>(cdl::ADD),
// 		  //the receiver object
// 		  new MemberFunctionChangeReceiver<FunnyMan>(this,
// 							     &FunnyMan::newOneLinerAdded));  
		  
//   //add this receiver to listen for changes
//   addChangeFilter(//listen for joke types
// 		  //that are added
// 		  //that are local to this subarchitecture
// 		  createLocalTypeFilter<Joke>(cdl::ADD),
// 		  //the receiver object
// 		  new MemberFunctionChangeReceiver<FunnyMan>(this,
// 							     &FunnyMan::newJokeAdded));  
		  



}



void FunnyMan::newJokeAdded(const cdl::WorkingMemoryChange & _wmc) {

    JokePtr joke
      = getMemoryEntry<Joke>(_wmc.address);
    println("got joke");

}

void FunnyMan::newOneLinerAdded(const cdl::WorkingMemoryChange & _wmc) {

  OneLinerPtr joke
    = getMemoryEntry<OneLiner>(_wmc.address);
  println("got oneliner");

}

void FunnyMan::newTwoLinerAdded(const cdl::WorkingMemoryChange & _wmc) {

  // get the id of the changed entry
  string id(_wmc.address.id);

  // say something funny
  quip(id);

}


const string & FunnyMan::generatePunchline(const string &_setup) {
  return m_punchline;
}

void FunnyMan::quip(const std::string & _id) {
  
  log("joke has been overwritten %i times", this->getVersionNumber(_id, m_subarchitectureID));

  //create a new copy of the data so we can change it
  
//   vector<TwoLinerPtr> entries;
//   getMemoryEntries<TwoLiner>(entries, 1);
//   log("entry size: %d", entries.size());


  lockEntry(_id, cdl::LOCKEDODR);

  println("locked");

  TwoLinerPtr joke
    = getMemoryEntry<TwoLiner>(_id);


  // work out a smarty-pants punchline
  const string & punchline(generatePunchline(joke->setup));

  // time it right
  println("*cough*");
  joke->punchline = punchline;

  // now write this back into working memory, this will manage the
  // memory for us
  overwriteWorkingMemory(_id, joke);

  unlockEntry(_id);  
}
  

void 
FunnyMan::runComponent() {
  while(isRunning()) {
    log("receieved a change");
    cast::cdl::CASTTime ct = getCASTTime();  
    waitForChanges();
  }
   
}


