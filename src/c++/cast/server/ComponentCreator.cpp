#include "ComponentCreator.hpp"

#include <iostream>

#ifndef __APPLE__
#include <linux/limits.h>
#endif

using namespace std;
using namespace Ice;
using namespace IceUtil;


namespace cast {

using namespace cdl;



/**
 * Load a shared library and return handle.
 */
void* loadLibrary(const string & _baseName)  {
  char libName[PATH_MAX];
  void *libHandle;

#ifdef __APPLE__
  snprintf(libName, PATH_MAX, "lib%s.dylib", _baseName.c_str());
#else
  snprintf(libName, PATH_MAX, "lib%s.so", _baseName.c_str());
#endif

  // note: multiple calls of dlopen() on the same library are ok and just
  // return the same handle
  // IMPORTANT: RTLD_LOCAL is crucial!
  // If RTLD_GLOBAL is used here, symbols of the loaded library (= component)
  // are made available globally. If two libraries contain the same symbols
  // (e.g.  Matrix::mult(), Image::Image()) one overwrites the other and one
  // library is left with a mess. The result is usually a very odd segfault
  // wich is very hard to find.
  

  libHandle = dlopen(libName, RTLD_NOW | RTLD_LOCAL);

  //nah: Changed to GLOBAL because symbols are not shared when opened via JNI
  //http://gcc.gnu.org/bugzilla/show_bug.cgi?id=11223
  //http://gcc.gnu.org/bugzilla/show_bug.cgi?id=4993
  //libHandle = dlopen(libName, RTLD_NOW | RTLD_GLOBAL);

  //cout<<"about to dlopen: "<<libName<<endl;
  
  //libHandle = dlopen(libName, RTLD_LAZY | RTLD_GLOBAL);

  //nah: bug fix #34
  //nah: changed back.... anything else seems to result in a hang

  //this causes the cast crash
  //libHandle = dlopen(libName, RTLD_NOW | RTLD_LOCAL);

  //this causes the freeze
  //libHandle = dlopen(libName, RTLD_NOW | RTLD_GLOBAL);
  
  //this causes the cast crash
  //libHandle = dlopen(libName, RTLD_LAZY | RTLD_LOCAL);

  //this causes the freeze
  //libHandle = dlopen(libName, RTLD_LAZY | RTLD_GLOBAL);

  //this causes the cast crash
  //libHandle = dlopen(libName, RTLD_LAZY);
  //libHandle = dlopen(libName, RTLD_NOW);

  if(!libHandle) {
    throw CASTException(exceptionMessage(__HERE__, dlerror()));
  }

  return libHandle;

}

void DynamicComponentCreator::splitComponentName(const std::string &_fullName,
    /*out*/ std::string& libName, /*out*/ std::string& componentName)
{
    int pos = _fullName.find("#");
    if (pos == string::npos) {
      libName = _fullName;
      componentName = "";
    }
    else {
      libName = _fullName.substr(0, pos);
      componentName = _fullName.substr(pos+1);
    }
}

cast::CASTComponentPtr DynamicComponentCreator::createNewComponent(
      const std::string &_procName, const std::string &_typeName) const
{
  //printf (" ***** CREATING '%s' of type '%s' *****\n", _procName.c_str(), _typeName.c_str());

  string libName, componentName;
  DynamicComponentCreator::splitComponentName(_typeName, libName, componentName);
  cast::CASTComponentPtr  proc = 0;
  if (componentName.length() < 1) {
    if (! m_newComponent) {
      throw CASTException(exceptionMessage(__HERE__,
            "Failed to load component '%s'. No function newComponent() defined in library lib%s.so", 
            _procName.c_str(), libName.c_str()));
    }
    proc = m_newComponent();
  }
  else {
    if (! m_newNamedComponent) {
      throw CASTException(exceptionMessage(__HERE__,
            "Failed to load component '%s' (%s). No function newNamedComponent(char*) defined in library lib%s.so", 
            _procName.c_str(), _typeName.c_str(), libName.c_str()));
    }
    proc = m_newNamedComponent(componentName.c_str());
  }

  if (! proc) {
    throw cast::CASTException(exceptionMessage(__HERE__, 
          "Failed to load component '%s' (%s)", 
          _procName.c_str(), _typeName.c_str()));
  }
  proc->setID(_procName, ::Ice::Current());

  return proc;
};
 
DynamicComponentCreator* DynamicComponentCreator::createComponentCreator(const std::string &_baseName)
   throw (CASTException)
{

  // mmarko: add support for multiple components in a single library
  string libName, componentName;
  DynamicComponentCreator::splitComponentName(_baseName, libName, componentName);

  //load the library
  void *libHandle = loadLibrary(libName);

  //pointer to the function for creating the component
  //cast::CASTComponentPtr (*newComponent)();
  //DynamicComponentCreator::NewComponentFn newComponent;
  //DynamicComponentCreator::NewNamedComponentFn newNamedComponent;

  //void pointer to the method
  void *nc = dlsym(libHandle, "newComponent");
  void *nnc = dlsym(libHandle, "newNamedComponent");

  //newComponent = (cast::CASTComponentPtr (*)(const string &))(nc);
  //newComponent = (cast::CASTComponentPtr (*)())(nc);

  //if(newComponent == 0) {
  if( nc == 0 && nnc == 0 ) {
    throw CASTException(exceptionMessage(__HERE__,
          "no function newComponent() or newNamedComponent(char*) defined in library lib%s.so", 
          libName.c_str()));
  }

  //create a new dynamic creator object using the dynamically loaded
  //library function pointer 
  return new DynamicComponentCreator(libHandle,
      DynamicComponentCreator::NewComponentFn(nc),
      DynamicComponentCreator::NewNamedComponentFn(nnc));
  //newComponent);
}




}; //namespace cast
