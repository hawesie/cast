/*
 * CAST - The CoSy Architecture Schema Toolkit
 *
 * Copyright (C) 2006-2009 Nick Hawes
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

#ifndef CAST_CAST_COMPONENT_H_
#define CAST_CAST_COMPONENT_H_

#include <cast/slice/CDL.hpp>
#include <cast/core/CASTUtils.hpp>
#include <cast/core/ComponentLogger.hpp>

#include <cstdarg>
#include <string>
#include <map>

#include <IceUtil/Thread.h> 
#include <IceUtil/IceUtil.h>
#include <Ice/ObjectAdapter.h> 
#include <Ice/Communicator.h> 

#include <log4cxx/logger.h>


namespace cast {
	
	
  //fwd decl
  class CASTComponent;
	
  typedef IceInternal::Handle<cast::CASTComponent> CASTComponentPtr;
	
  /**
   * Thread class to run a component
   */
	
  class ComponentRunThread :
    public IceUtil::Thread {
  public:
    ComponentRunThread(CASTComponent *_comp);
    virtual void run();
		
  private:
    CASTComponent * m_component;
		
  };
	
  /**
   * A basic component in the CAST framework. This class provides some
   * basic functionality useful for running, printing and debugging. The
   * component is not associated with a subarchitecture, so could be
   * used for components that run externally to a CAST architecture
   * (e.g. sensor).
   * 
   * @author nah
   */
  class CASTComponent : 
    public virtual cast::interfaces::CASTComponent {
		
  private:
		
    /**
     * The ID of this component. This is currently protected by this class
     * to ensure it remains unchanged after process construction.
     */
    std::string m_componentID;
		
//     std::ostream & 
//     startColourise(std::ostream &_stream) const;
		
//     std::ostream & 
//    endColourise(std::ostream &_stream) const;
		
    std::string m_startColourEscape;
    static const std::string END_COLOUR_ESCAPE;
		
    bool m_startCalled;
    bool m_configureCalled;
		
    //level configured with
    log4cxx::LevelPtr m_logLevel;
    //extra stuff send to logger
    core::logging::LogAdditions m_additions;
	
    //thread in which component will run
    IceUtil::ThreadPtr m_runThread;
    //control for the above thread
    IceUtil::ThreadControl m_runThreadControl;
    IceUtil::Mutex m_componentMutex;
		
    ///the object adapter which is serving this component
    Ice::ObjectAdapterPtr m_adapter;
    ///the identity which Ice is using for the current instance
    Ice::Identity m_iceIdentity;
		
		
    ///proxy for cast component manager
    interfaces::ComponentManagerPrx m_manager;
		
		
    ///proxy for cast time server
    interfaces::TimeServerPrx m_timeServer;
		
    
    cast::CASTComponentPtr m_componentPtr;

    //list containing identities of any additional registered servers
    std::vector<Ice::Identity> m_serverIdentities;
		

  public:
		
    /**
     * Construct a new component with the given ubnique ID.
     * 
     * @param _id
     *            The id used to identify this component.
     */
    CASTComponent();
		
    /**
     * Empty virtual destructor;
     */
    virtual ~CASTComponent();
		
		
    virtual void setID(const std::string & _id,
		       const Ice::Current & _ctx);

    virtual void start(const Ice::Current & _ctx);
		
    virtual void run(const Ice::Current & _ctx);
		
    virtual void stop(const Ice::Current & _ctx);
		
		
    virtual void destroy(const Ice::Current&);
		
    /**
     * Get the process identifier of this process.
     */
    const std::string & getProcessIdentifier() const  __attribute__ ((deprecated)) {
      return getComponentID();
    }
		
    /**
     * Get the unique identifier associated with this component.
     */
    const std::string & getComponentID() const {
      return m_componentID;
    }
		
    virtual 
    std::string 
    getID(const ::Ice::Current& _crt) {
      return getComponentID();
    }
		
		
    ///The following methods are from CDL.ice and implement the
    ///component contract
		
		
    virtual void beat(const Ice::Current & _current) const {}
    //virtual void run(const Ice::Current & _current){}
    //virtual void start(const Ice::Current & _current){}
		
    virtual 
    void configure(const cdl::StringMap & _config, const Ice::Current & _current);		
		
    virtual 
    void 
    setComponentManager(const cast::interfaces::ComponentManagerPrx & _man, 
			const ::Ice::Current & _ctx) {
      m_manager = _man;
    }

		
    /**
     * Get the manager component used for this component.
     */
    interfaces::ComponentManagerPrx getComponentManager() const {
      return m_manager;
    }

    virtual 
    void 
    setTimeServer(const cast::interfaces::TimeServerPrx & _ts, 
		  const ::Ice::Current & _ctx) {
      m_timeServer = _ts;
    }
		
    void
    _setObjectAdapter(const Ice::ObjectAdapterPtr & _adapter) {
      m_adapter = _adapter;
    }
		
		
    Ice::ObjectAdapterPtr 
    getObjectAdapter() const {
      assert(m_adapter);
      return m_adapter;
    }
		
    Ice::CommunicatorPtr
    getCommunicator() const {
      assert(m_adapter);
      return m_adapter->getCommunicator();
    }
		
		
    void
    _setIceIdentity(const Ice::Identity & _id) {
      m_iceIdentity = _id;
    }
		
		
    const Ice::Identity &
    getIceIdentity() const {
      return m_iceIdentity;
    }
		
    /**
     * Print out the input in a formatted way.
     * 
     * @param _s
     *            Std::String to print.
     */
    virtual void println(const std::string & _s) const;
		
		
    /**
     * printf-like method
     * @param format   (in) printf-style format std::string
     */
    virtual void println(const char *format, ...) const;

    /**
     * Print out the input in a formatted way to error log.
     * 
     * @param _s
     *            Std::String to print.
     */
    virtual void error(const std::string & _s) const;
		
		
    /**
     * printf-like method for error log.
     * @param format   (in) printf-style format std::string
     */
    virtual void error(const char *format, ...) const;


		
    /**
     * Returns true while the component is running (i.e. post start())
     */
    bool isRunning() const {
      return m_startCalled;
    };
	
    
    /**
     * Set the component's smart pointer to itself. Internal use only.
     */
    void _setComponentPointer(cast::CASTComponentPtr _ptr) {
      assert(!m_componentPtr);
      //we are one and the same
      assert(&(*_ptr) == this);
      m_componentPtr = _ptr;
    }


    /**
     * Log a stl std::string. Only does anything if the variable m_bLogOutput
     * is true.
     *
     * @param _s The stl std::string to log.
     */  
    virtual void log(const std::string & _s) const;
		
    /**
     * printf-like method
     * @param format   (in) printf-style format std::string
     */
    virtual void log(const char *format, ...) const;
		
		
    /**
     * Use an stl std::string as debug output. Only does anything if the
     * variable m_bDebugOuput is true.
     *
     * @param _s The stl std::string to log.
     */  
    virtual void debug(const std::string & _s) const;
		
    /**
     * printf-like method
     * @param format   (in) printf-style format std::string
     */
    virtual void debug(const char *format, ...) const;
		
    /**
     * Acquire the semaphore for access to this component. Use
     * lockComponent instead.
     */
    void lockProcess() __attribute__ ((deprecated)) {
      lockComponent();
    }
		
    void lockComponent();
		
    /**
     * Release the semaphore for access to this component. Use
     * unlockComponent instead.
     */
    void unlockProcess() __attribute__ ((deprecated)) {
      unlockComponent();
    }
		
    void unlockComponent();

    typedef log4cxx::helpers::ObjectPtrT<core::logging::ComponentLogger> ComponentLoggerPtr;
	
    /**
     * Get the logger used by this component.
     */
    virtual ComponentLoggerPtr getLogger() {
      if (m_logger == NULL) {
	m_logger = core::logging::ComponentLogger::getLogger(getLoggerName());
	if (m_logger) {
	  if (m_logLevel != NULL) {
	    m_logger->setLevel(m_logLevel);
	  }
	}
	else {
	  m_logger = getLogger(".main");
	}
	if (! m_logger) {
	  printf("**** LOGGER '%s' CREATION FAILED ****\n", getLoggerName().c_str());
	}

      }
      return m_logger;
    }

    /**
     * Get a logger with a name that extends the component logger name.
     * 
     * @param _postfix
     *            String to be directly appended to base logger name.
     */
    virtual ComponentLoggerPtr getLogger(const std::string & _postfix) {
      return core::logging::ComponentLogger::getLogger(getLoggerName() + _postfix);
    }


    virtual const core::logging::LogAdditions & getLogAdditions() {

      if(m_additions.getComponentID().empty()) {
	m_additions = core::logging::LogAdditions(getComponentID(), "", getColourEscape());
      }
      return m_additions;
    }

    /**
     * Get the current CAST time. This is a monotomic timer that starts at 0 on startup.
     */
    cdl::CASTTime getCASTTime() const {
      assert(m_timeServer);
      return m_timeServer->getCASTTime();
    }
        
    /**
     * Get the time server used by this component. Can be used to perform more advanced time calculations.
     */
    const interfaces::TimeServerPrx getTimeServer() const {
      return m_timeServer;   
    }


    
    /**
     * Converts an Ice type to a string used for its category in in a
     * server. ::some::type::Name becomes some.type.Name
     */
    template <class InterfaceType> 
    std::string toServantCategory() const {

      //we need to convert this to a Java class name structure
      const std::string &iceId(InterfaceType::ice_staticId());

      //remove leading :s
      std::string typeString;
      unsigned int colonCount = 0;

      //start past the leading ::
      for(std::string::size_type i = 2;
	  i < iceId.length(); ++i) {
	//if a colon and the first in a pair (i.e. even count)
	if(iceId[i] == ':') {
	  if(colonCount % 2 == 0) {
	    typeString += '.';
	  }
	  colonCount++;
	}
	else {
	  typeString += iceId[i];
	}

      }
      
      return typeString;

    }

    /**
     * Registers the provided servant as a Ice server that can offer the
     * provided interface. Servants are automatically removed at shutdown.
     * 
     * @param <Type>
     * @param name
     * @param category
     * @param _servant
     */        
    template <class PointerType> 
    void registerIceServer(const std::string & _name, 
			      const std::string & _category, 
			      IceInternal::Handle<PointerType> _servant) {
      Ice::Identity id;
      id.name = _name;
      id.category = _category;
      getObjectAdapter()->add(_servant, id);
      m_serverIdentities.push_back(id);
    }    
    

    /**
     * Resolve an ice server provided by the given component providing the
     * supplied interface.
     * 
     * @param <PointerType>
     * @param _servant
     */
    template <class PointerType, class InterfaceType> 
    void registerIceServer(IceInternal::Handle<PointerType> _servant) {   
      std::string category(toServantCategory<InterfaceType>());
      registerIceServer(getComponentID(),
			   category,
			   _servant);
    }
   
      
  /**
   * Resolve an Ice server using the given details.
   */
  Ice::ObjectPrx getIceServer(const std::string & _name, const std::string & _category, 
			      const std::string & _host, unsigned int _port) const;  
  
  /**
   * Resolve an Ice server provided by the given component providing
   * the supplied interface. This relies on the server being
   * registered with a registerIceServer call.  
   */
  template <typename InterfaceType> 
  typename InterfaceType::ProxyType getIceServer(const std::string & _componentID) const {
    
    cdl::ComponentDescription desc = getComponentManager()->
      getComponentDescription(_componentID);

    const std::string & host(desc.hostName);
    unsigned int port = cast::languageToPort(desc.language);

    Ice::ObjectPrx prx = getIceServer(_componentID, toServantCategory<InterfaceType>(), host, port);

    typename InterfaceType::ProxyType checkedPrx  = InterfaceType::ProxyType::checkedCast(prx);

    if(checkedPrx) {
      return checkedPrx;
    }
    else {
      throw CASTException(exceptionMessage(__HERE__, 
					   "Unable to cast returned proxy to %s", 
					   typeName<InterfaceType>().c_str())); 
    }
    
  }



  protected:
		
    virtual std::string getLoggerName() {
      return getComponentID();
    }


    const std::string & getColourEscape() {
      return m_startColourEscape;
    }

    virtual void destroy() {}
    virtual void destroyInternal(const Ice::Current&);
	
    /**
     * Called when component should be stopped. Makes isRunning() false.
     */
    virtual void stop();
		
    virtual void stopInternal();
		
    /**
     * Overrides the configure method from FrameworkProcess to use
     * _config to set the subarchitecture ID.
     *
     * Typically overloaded by the subclasses to do the configuration
     * 
     * @param _config A map of config values.
     */
    virtual void configure(const std::map<std::string,std::string> & _config);
		
    /**
     * Configures internal CAST stuff
     * @param _config A map of config values.
     *
     * Typically overloaded by the subclasses to perform things that
     * should be done once after configuration but before the
     * components starts running, i.e. runComponent is called.
     * 
     */
    virtual void configureInternal(const std::map<std::string,std::string> & _config);
		
    /**
     * Called on component start, after configure, before
     * runComponent. At this stage all connections are complete.
     *
     * Typically overloaded by the subclasses to perform things that
     * should be done once after configuration but before the
     * components starts running, i.e. runComponent is called.
     * 
     */
    virtual void start();
		
    virtual void startInternal();
		
		
		
		
		
    friend class ComponentRunThread;
		
    /**
     * Method called in separate thread to run processing component.
     * This method is called for each component when it is started by
     * the framework process server.
     */
    virtual void runComponent() {};
		
		
    //     /**
    //      * Wait until the unlockProcess is successfully called.
    //      */
    //     void waitForUnlock();
		
    /**
     * Put the processes thread to sleep for a number of
     * milliseconds. Use sleep component instead.
     * 
     * @param _millis
     *            Number of milliseconds to sleep for.
     */
    void sleepProcess(unsigned long _millis) __attribute__ ((deprecated)) {
      sleepComponent(_millis);
    }
		
        
    /**
     * Put the calling thread to sleep for a number of
     * milliseconds. 
     * @param _millis
     *            Number of milliseconds to sleep for.
     */
    void sleepComponent(unsigned long _millis);
		

    ///Controls log output
    bool m_bLogOutput;
    ///Controls debug output
    bool m_bDebugOutput;



    ///this is a smart pointer to the current component, as used by
    ///Ice's object adapter. using this allows you to pass around a
    ///pointer to this component safely to ice to be used as a servant
    ///for something else
    cast::CASTComponentPtr getComponentPointer() {
      assert(m_componentPtr);
      return m_componentPtr;
    }

    ComponentLoggerPtr m_logger;    

    /***
     * A class that locks a CASTComponent on construction and unlocks it on
     * destruction, eg. when going out of scope.
     *
     * @see IceUtil::Mutex, IceUtil::Mutex::Lock
     *
     * Example:
     *   a) an error can keep a component locked indefinitely
     *    void MyComponent::BadExample() {
     *      lockComponent();
     *      throw runtime_error("something went wrong");
     *      unlockComponent(); // the component is never unlocked
     *    }
     *
     *   b) the lock is removed on error
     *    void MyComponent::GoodExample() {
     *      CASTComponent::Lock lock(this);
     *      throw runtime_error("something went wrong");
     *      // lock is destroyed during stack unwinding and the component is unlocked.
     *    }
     */
    class Lock
      : private IceUtil::Mutex::Lock
    {
      public:
	Lock(CASTComponent* pComponent)
	  : IceUtil::Mutex::Lock(pComponent->m_componentMutex)
	{
	}
    };
  };

} //namspace cast


#endif
