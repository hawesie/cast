#ifndef COMPONENT_CREATOR_HPP_
#define COMPONENT_CREATOR_HPP_

#include <Ice/Ice.h> 

#include <cast/slice/CDL.hpp> 
#include <cast/core/CASTUtils.hpp> 
#include <cast/core/CASTComponent.hpp> 
#include <cast/core/StringMap.hpp> 
#include <dlfcn.h>
#include <string>

namespace cast {

// 2012-01-20 (mmarko): added support for multiple components in a single library file
//    COMPONENT CPP unnamed ExampleComponent
//    COMPONENT CPP named.1 ExampleComponent#Example1
//    # Both components reside in libExampleComponent
//    # unnamed is created with a call to newComponent()
//    # named.1 is created with a call to newNamedComponent("Example1")
class DynamicComponentCreator {
private:
  typedef cast::CASTComponentPtr (*NewComponentFn)();
  typedef cast::CASTComponentPtr (*NewNamedComponentFn)(const char*);

  ///Handle on the  the dynamically loaded library the component comes from
  void * m_libHandle;

  // pointers 
  NewComponentFn m_newComponent;
  NewNamedComponentFn m_newNamedComponent;

 public:

  DynamicComponentCreator(void * _libHandle,
      NewComponentFn _newComponent,
      NewNamedComponentFn _newNamedComponent
      //cast::CASTComponentPtr (*_newComponent)(),
      //cast::CASTComponentPtr (*_newNamedComponent)()
      )
  {
    m_libHandle = _libHandle;
    m_newComponent = _newComponent;
    m_newNamedComponent = _newNamedComponent;
  }

  ///Closes the library that was opened to create this 
  virtual ~DynamicComponentCreator() {
    dlclose(m_libHandle);
  }

  virtual cast::CASTComponentPtr createNewComponent(const std::string &_procName,
        const std::string &_typeName) const;

  static void splitComponentName(const std::string &_baseName,
      /*out*/ std::string& libName, /*out*/ std::string& componentName);

  static DynamicComponentCreator* createComponentCreator(const std::string &_baseName)
     throw (CASTException);
};

  void* loadLibrary(const std::string & _baseName);

}; //namespace cast


#endif
