#include "CASTComponentFactory.hpp"

#include <CDL.hpp>
#include <ComponentCreator.hpp>
#include <iostream>


using namespace Ice;
using namespace cast::cdl;
using namespace cast::interfaces;
using namespace std;

namespace cast {

  
  cast::interfaces::CASTComponentPrx 
  CASTComponentFactory::createBase(const ::std::string& id, const ::std::string& type, const ::Ice::Current & _crt) 
    throw (ComponentCreationException)
  {

    try {
      //get creator
      DynamicComponentCreator * dcc(NULL);
      string libName, componentName;
      DynamicComponentCreator::splitComponentName(type, libName, componentName);
      ComponentCreatorMap::iterator i = m_creators.find(libName);
      //if we don't have it yet
      if(i == m_creators.end()) {
	dcc = DynamicComponentCreator::createComponentCreator(type);
	m_creators[type] = dcc;       
     }
      else {
	dcc =  i->second;
      }
      
      Identity iceid;
      iceid.name = id;
      iceid.category = type;    
      
      cast::CASTComponentPtr component = dcc->createNewComponent(id, type);
      component->_setObjectAdapter(_crt.adapter);
      component->_setIceIdentity(iceid);
      component->_setComponentPointer(component);
      
      ObjectPrx base = _crt.adapter->add(component,iceid);
      
      return CASTComponentPrx::uncheckedCast(base);
    }
    catch (const CASTException & e) {
      cerr<<"Exception when creating component "<<id<<" of type "<<type<<endl;
      cerr<<e.message<<endl;
      ComponentCreationException cce(e.message);
      throw cce;
    }    
  }
  
  cast::interfaces::CASTComponentPrx CASTComponentFactory::newComponent(const ::std::string& id, const ::std::string& type, bool newProcess, const ::Ice::Current & _crt) 
    throw (ComponentCreationException) {
    return createBase(id,type,_crt);
  }
  
  cast::interfaces::ManagedComponentPrx CASTComponentFactory::newManagedComponent(const ::std::string& id, const ::std::string& type, bool newProcess, const ::Ice::Current & _crt) 
    throw (ComponentCreationException) {
    return ManagedComponentPrx::checkedCast(createBase(id,type,_crt));
  }
  
  cast::interfaces::UnmanagedComponentPrx CASTComponentFactory::newUnmanagedComponent(const ::std::string& id, const ::std::string& type, bool newProcess, const ::Ice::Current & _crt) 
    throw (ComponentCreationException) {
    return UnmanagedComponentPrx::checkedCast(createBase(id,type,_crt));    
  }
  
  cast::interfaces::WorkingMemoryPrx CASTComponentFactory::newWorkingMemory(const ::std::string& id, const ::std::string& type, bool newProcess, const ::Ice::Current & _crt) 
    throw (ComponentCreationException) {
    return WorkingMemoryPrx::checkedCast(createBase(id,type,_crt));
  }
  
  cast::interfaces::TaskManagerPrx CASTComponentFactory::newTaskManager(const ::std::string& id, const ::std::string& type, bool newProcess, const ::Ice::Current & _crt)     
    throw (ComponentCreationException) {
    return TaskManagerPrx::checkedCast(createBase(id,type,_crt));
  }
  
};
