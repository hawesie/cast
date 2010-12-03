/*
 * CAST - The CoSy Architecture Schema Toolkit
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
 
#include "CASTComponent.hpp"

using namespace std;
using namespace Ice;

namespace cast {

  //daft, but matches java for log4* config
  using namespace core::logging;

  ComponentRunThread::ComponentRunThread(CASTComponent *_comp) : m_component(_comp) {}
  
  void 
  ComponentRunThread::run() {
    try {
      m_component->runComponent();
    }
    catch(const WMException &e) {
      m_component->error("***************************************************************");
      m_component->error("***************************************************************");

      m_component->error("Aborting after catching a WMException from runComponent()");
      m_component->error("The address involved in this error was: " + 
			   e.wma.id + " in " + 
			   e.wma.subarchitecture);	  		
      m_component->error("what(): %s", e.what());
      m_component->error("message: %s", e.message.c_str());
      m_component->error("***************************************************************");
      m_component->error("***************************************************************");

      std::abort();
    }    
    catch(const cast::CASTException & e) {
      m_component->error("***************************************************************");
      m_component->error("***************************************************************");
      
      m_component->error("Aborting after catching a CASTException from runComponent()");
      m_component->error("what(): %s", e.what());
      m_component->error("message: %s", e.message.c_str());


      m_component->error("***************************************************************");
      m_component->error("***************************************************************");

      std::abort();

    }
    catch(const Ice::Exception &e) {

      m_component->error("***************************************************************");
      m_component->error("***************************************************************");

      m_component->error("Aborting after catching an Ice::Exception from runComponent()");
      m_component->error("what(): %s", e.what());

      m_component->error("***************************************************************");
      m_component->error("***************************************************************");

      std::abort();
    }
    catch(const std::exception &e) {
      m_component->error("***************************************************************");
      m_component->error("***************************************************************");

      m_component->error("Aborting after catching std::exception from runComponent()");
      m_component->error("what(): %s", e.what());

      m_component->error("***************************************************************");
      m_component->error("***************************************************************");

      std::abort();
    }
    catch(...) {
      m_component->error("***************************************************************");
      m_component->error("***************************************************************");

      m_component->error("Aborting after catching unknown exception from runComponent()");
      m_component->error("***************************************************************");
      m_component->error("***************************************************************");

      std::abort();
    }


  }


  const std::string CASTComponent::END_COLOUR_ESCAPE = "\033[0m";

  CASTComponent::CASTComponent() : 
    m_componentID(""),
    m_startColourEscape(END_COLOUR_ESCAPE),
    m_startCalled(false),
    m_configureCalled(false),
    m_bLogOutput(false),
    m_bDebugOutput(false),
    m_logLevel(),
    m_logger(ComponentLogger::getLogger("cast.init")) {

  }


  CASTComponent::~CASTComponent() {

    //    error("CASTComponent::~CASTComponent()");
//UPGRADE
	  //    m_unlockNotificationMutex.lock();
//    m_pUnlockNotificationCondition->broadcast();
//    m_unlockNotificationMutex.unlock();
//    delete m_pUnlockNotificationCondition;
  }



  void 
  CASTComponent::run(const Ice::Current & _ctx) {
    assert(m_startCalled);
    m_runThread = new ComponentRunThread(this);
    //start thread to run component  
    //create a new thread
    m_runThreadControl = m_runThread->start();
  }

  
  void 
  CASTComponent::stop(const Ice::Current & _ctx)  {
  
    assert(m_startCalled);
    m_startCalled = false;
    
    lockComponent();
    
    try {
      //stop derived class
      stop();
      
      //stop internal processing
      stopInternal();
    }
    catch(const WMException &e) {
      error("***************************************************************");
      error("***************************************************************");
      error("Aborting after catching a WMException from runComponent()");
      error("The address involved in this error was: " + 
	      e.wma.id + " in " + 
	      e.wma.subarchitecture);	  		
      error("what(): %s", e.what());
      error("message: %s", e.message.c_str());
      error("***************************************************************");
      error("***************************************************************");
      
      std::abort();
    }    
    catch(const cast::CASTException & e) {
      error("***************************************************************");
      error("***************************************************************");      
      error("Aborting after catching a CASTException from runComponent()");
      error("what(): %s", e.what());
      error("message: %s", e.message.c_str());

      error("***************************************************************");
      error("***************************************************************");

      std::abort();

    }
    catch(const Ice::Exception &e) {

      error("***************************************************************");
      error("***************************************************************");
      error("Aborting after catching an Ice::Exception from runComponent()");
      error("what(): %s", e.what());
      error("***************************************************************");
      error("***************************************************************");

      std::abort();
    }
    catch(const std::exception &e) {
      error("***************************************************************");
      error("***************************************************************");
      error("Aborting after catching std::exception from runComponent()");
      error("what(): %s", e.what());
      error("***************************************************************");
      error("***************************************************************");

      std::abort();
    }
    catch(...) {
      error("***************************************************************");
      error("***************************************************************");
      error("Aborting after catching unknown exception from runComponent()");
      error("***************************************************************");
      error("***************************************************************");

      std::abort();
    }

    
    unlockComponent();
  }
  
  void CASTComponent::stop() {
  }

  void CASTComponent::stopInternal() {
    debug("trying to join runComponent()");
    m_runThreadControl.join();

    //UPGRADE
    //    //release sleeping threads
    //    m_unlockNotificationMutex.lock();
    //    m_pUnlockNotificationCondition->broadcast();
    //    m_unlockNotificationMutex.unlock();
  }


  void CASTComponent::println(const char *format, ...) const {
    char msg[1024];
    va_list arg_list;
    va_start(arg_list, format);
    vsnprintf(msg, 1024, format, arg_list);
    va_end(arg_list);
  
    CAST_INFO(m_logger, msg, m_additions);
  }

  void CASTComponent::println(const string & _s) const {
    println(_s.c_str());
  }


  void CASTComponent::error(const char *format, ...) const {
    char msg[1024];
    va_list arg_list;
    va_start(arg_list, format);
    vsnprintf(msg, 1024, format, arg_list);
    va_end(arg_list);
  
    CAST_ERROR(m_logger, msg, m_additions);
  }

  void CASTComponent::error(const string & _s) const {
    error(_s.c_str());
  }



  void CASTComponent::log(const char *format, ...) const {
    
    if(m_logger->isDebugEnabled()) {
      char msg[1024];
      va_list arg_list;
      va_start(arg_list, format);
      vsnprintf(msg, 1024, format, arg_list);
      va_end(arg_list);
      
      CAST_DEBUG(m_logger, msg, m_additions);    
    }
  }

  void CASTComponent::log(const string & _s) const {
    log(_s.c_str());
  }




  void CASTComponent::debug(const char * format, ...) const {
    if(m_logger->isTraceEnabled()) {
      char msg[1024];
      va_list arg_list;
      va_start(arg_list, format);
      vsnprintf(msg, 1024, format, arg_list);
      va_end(arg_list);
      
      CAST_TRACE(m_logger, msg, m_additions);
    }
  }

  void CASTComponent::debug(const string & _s) const {
    debug(_s.c_str());
  }



  void CASTComponent::setID(const std::string & _id,
			    const Ice::Current & _ctx)  {
    assert(m_componentID == "");
    m_componentID = _id;
  }
  
  void CASTComponent::start(const Ice::Current & _ctx)  {
			
    
    try {
      //start internals first
      startInternal();
			
      //start derived class next
      start();
    }
    catch(const WMException &e) {
      error("***************************************************************");
      error("***************************************************************");
      error("Aborting after catching a WMException from runComponent()");
      error("The address involved in this error was: " + 
			   e.wma.id + " in " + 
			   e.wma.subarchitecture);	  		
      error("what(): %s", e.what());
      error("message: %s", e.message.c_str());
      error("***************************************************************");
      error("***************************************************************");

      std::abort();
    }    
    catch(const cast::CASTException & e) {
      error("***************************************************************");
      error("***************************************************************");      
      error("Aborting after catching a CASTException from runComponent()");
      error("what(): %s", e.what());
      error("message: %s", e.message.c_str());
      error("***************************************************************");
      error("***************************************************************");

      std::abort();

    }
    catch(const Ice::Exception &e) {

      error("***************************************************************");
      error("***************************************************************");
      error("Aborting after catching an Ice::Exception from runComponent()");
      error("what(): %s", e.what());
      error("***************************************************************");
      error("***************************************************************");

      std::abort();
    }
    catch(const std::exception &e) {
      error("***************************************************************");
      error("***************************************************************");
      error("Aborting after catching std::exception from runComponent()");
      error("what(): %s", e.what());
      error("***************************************************************");
      error("***************************************************************");
      std::abort();
    }
    catch(...) {
      error("***************************************************************");
      error("***************************************************************");
      error("Aborting after catching unknown exception from runComponent()");
      error("***************************************************************");
      error("***************************************************************");

      std::abort();
    }
  }

  void CASTComponent::start() {
  }

  void CASTComponent::startInternal() {
    debug("CASTComponent::startInternal()");
    assert(!m_startCalled);
    m_startCalled = true;
  }

		
  
  void CASTComponent::configure(const cdl::StringMap & _config, const Ice::Current & _current) {
	
    
    try {		
      //configure internals
      configureInternal(_config);
      
      //some logging mojo
      //reset so new name is used after internal config
      //set to empty
      m_logger = ComponentLoggerPtr();
      //now reset
      m_logger = getLogger();
      m_additions = getLogAdditions();

      //configure derived class
      configure(_config);
    }
    catch(const WMException &e) {
      error("***************************************************************");
      error("***************************************************************");
      error("Aborting after catching a WMException from runComponent()");
      error("The address involved in this error was: " + 
	      e.wma.id + " in " + 
	      e.wma.subarchitecture);	  		
      error("what(): %s", e.what());
      error("message: %s", e.message.c_str());
      error("***************************************************************");
      error("***************************************************************");
      
      std::abort();
    }    
    catch(const cast::CASTException & e) {
      error("***************************************************************");
      error("***************************************************************");      
      error("Aborting after catching a CASTException from runComponent()");
      error("what(): %s", e.what());
      error("message: %s", e.message.c_str());

      error("***************************************************************");
      error("***************************************************************");

      std::abort();

    }
    catch(const Ice::Exception &e) {

      error("***************************************************************");
      error("***************************************************************");
      error("Aborting after catching an Ice::Exception from runComponent()");
      error("what(): %s", e.what());
      error("***************************************************************");
      error("***************************************************************");

      std::abort();
    }
    catch(const std::exception &e) {
      error("***************************************************************");
      error("***************************************************************");
      error("Aborting after catching std::exception from runComponent()");
      error("what(): %s", e.what());
      error("***************************************************************");
      error("***************************************************************");

      std::abort();
    }
    catch(...) {
      error("***************************************************************");
      error("***************************************************************");
      error("Aborting after catching unknown exception from runComponent()");
      error("***************************************************************");
      error("***************************************************************");

      std::abort();
    }
    }
		
		

  void CASTComponent::configure(const map<string,string> & _config) {
  }

  void CASTComponent::configureInternal(const map<string,string> & _config) {

//     cout<<"CASTComponent::configure: "<<endl;

//     for(map<string,string>::const_iterator cf = _config.begin();
// 	cf != _config.end(); ++cf) {
//       cout<<"config: "<<cf->first<<" "<<cf->second<<endl;
//     }
    

    
    m_configureCalled = true;

    map<string,string>::const_iterator i = _config.find(cdl::LOGKEY);

    if(i != _config.end()) {
      string logValue = i->second;
      if(logValue == "true") {
	m_bLogOutput = true;
	m_logLevel = Level::getDebug();
      }
      else if(logValue == "false") {
	m_bLogOutput = false;
      }
      else {
	error(string("config err, unknown value for log") + logValue); 
      }   
    }

    i = _config.find(cdl::DEBUGKEY);

    if(i != _config.end()) {
      string logValue = i->second;
      if(logValue == "true") {
	m_bDebugOutput = true;
	m_logLevel = Level::getTrace();
      }
      else if(logValue == "false") {
	m_bDebugOutput = false;
      }
      else {
	error(string("config err, unknown value for log") + logValue); 
      }   
    }

    i = _config.find(cdl::COMPONENTNUMBERKEY);

    assert(i != _config.end());
    string numberString = i->second;
    int myNumber = atoi(numberString.c_str());
    int printNumber = (myNumber % 7);
    int bold = (myNumber / 7) % 2;

    if(printNumber == 0) {
      //0 = do nothing
      m_startColourEscape = "\033[0m";
    }
    else {
      //otherwise use the connected colour
      ostringstream outStream;
      outStream<<"\033[3"<<printNumber;
      if(bold != 0) {
	outStream<<";1m";      
      }
      else {
	outStream<<"m";      
      }
      m_startColourEscape = outStream.str();
    }

  }

//   std::ostream & 
//   CASTComponent::startColourise(std::ostream &_stream) const {
//     _stream<<m_startColourEscape;
//     return _stream;
//   }
  
//   std::ostream & 
//   CASTComponent::endColourise(std::ostream &_stream) const {
//     _stream<<END_COLOUR_ESCAPE;
//     return _stream;
//   }


  void CASTComponent::lockComponent() {
    m_componentMutex.lock();
  }

  /**
   * Release the semaphore for access to this component.
   */
  void CASTComponent::unlockComponent() {

   //m_semaphore.post();
   m_componentMutex.unlock();

//    m_unlockNotificationMutex.lock();
//    m_pUnlockNotificationCondition->broadcast();
//    m_unlockNotificationMutex.unlock();  

  }


//   void CASTComponent::waitForUnlock() {
// //UPGRADE
// 	  //    m_unlockNotificationMutex.lock();
// //    m_pUnlockNotificationCondition->wait();
// //    m_unlockNotificationMutex.unlock();
//   }

  //#include <errno.hpp>

  void CASTComponent::sleepComponent(unsigned long _millis) {
    IceUtil::Time t = IceUtil::Time::milliSeconds(_millis); 
    IceUtil::ThreadControl::sleep(t);
  }
  
  void 
  CASTComponent::destroy(const Ice::Current & _crt) {
    destroy();
    destroyInternal(_crt);
  }
  
  void 
  CASTComponent::destroyInternal(const Ice::Current& _crt) {
    
    for(vector<Identity>::iterator i = m_serverIdentities.begin();
 	i < m_serverIdentities.end(); ++i) {
      getObjectAdapter()->remove(*i);
    }

    _crt.adapter->remove(_crt.id);
  }

  /**
   * Resolve an Ice server using the given details.
   */
  Ice::ObjectPrx 
  CASTComponent::getIceServer(const std::string & _name, const std::string & _category, 
			      const std::string & _host, unsigned int _port) const {
    
    Ice::Identity id;
    id.name = _name;
    id.category = _category;
    
    std::ostringstream serverAddr;
    serverAddr << getCommunicator()->identityToString(id)
	       << ":default -h " << _host << " -p " << _port;
    
    Ice::ObjectPrx base = getCommunicator()->stringToProxy(serverAddr.str());
    
    return base;
  }



} //namespace cast 
