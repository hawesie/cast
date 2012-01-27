
#include <CASTComponentFactory.hpp> 
#include <CASTTimeServer.hpp> 

#include <ComponentLayout.hpp> 
#include <Logging.hpp> 


#include <CDL.hpp> 
#include <Ice/Ice.h> 
 
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/file.h>
#include <log4cxx/helpers/pool.h>
#include <log4cxx/helpers/loglog.h>
#include <log4cxx/helpers/properties.h>
#include <log4cxx/stream.h>

#ifdef GOOGLE_PROFILER
#include <google/profiler.h>
#endif 

using namespace Ice;
using namespace std;
using namespace log4cxx;
using namespace log4cxx::helpers;

namespace cast {
  
  using namespace core::logging;
  using namespace cdl;

  typedef log4cxx::helpers::ObjectPtrT<core::logging::ComponentLogger> ComponentLoggerPtr;


  class ComponentServer : virtual public Ice::Application { 
  private:
    bool isProfiling;
  public: 

#ifdef GOOGLE_PROFILER
    virtual void interruptCallback(int i) {
      if (isProfiling)
        ProfilerStop();
      communicator()->destroy();
    }
#endif
    virtual int run(int _argc, char* _argv[]) { 
      //
      cast::core::logging::initLogging();

      ComponentLoggerPtr logger = ComponentLogger::getLogger("cast.server.c++.ComponentServer");
      CAST_INFO(logger, "CPP server version: \""<<cdl::CASTRELEASESTRING<<"\"", LogAdditions("cast.server.c++.ComponentServer","","")); 

      
      //streams seem to have memory issues
      //log4cxx::logstream logstream(logger, Level::getInfo());
      //logstream<<"CPP server version: \""<<cdl::CASTRELEASESTRING<<"\""<<LOG4CXX_ENDMSG;
     
#ifdef GOOGLE_PROFILER
      char* profileFName=getenv("CAST_PROFILER");
      if (profileFName!=NULL) {
        ProfilerStart(profileFName);
		isProfiling=true;
        callbackOnInterrupt();
        CAST_INFO(logger, "profiling enabled. profile file to be written: \""<<profileFName<<"\"", LogAdditions("cast.server.c++.ComponentServer","","")); 
      } else {
        isProfiling = false;
        CAST_INFO(logger, "profiling disabled. set env variable 'CAST_PROFILER=filename.prof' to enable it.", LogAdditions("cast.server.c++.ComponentServer","","")); 
      }
#else
      isProfiling = false;
      CAST_INFO(logger, "profiling disabled at compile time. recompile CAST with GOOGLE_PROFILER turned on.", LogAdditions("cast.server.c++.ComponentServer","","")); 
#endif

      CommunicatorPtr ic = communicator();
      
      char buf[50];
      snprintf(buf,50,"default -p %d", cast::cdl::CPPSERVERPORT);
      
      ObjectAdapterPtr adapter 
	= ic->createObjectAdapterWithEndpoints("ComponentServer1", buf);
      
      //add component factory to server
      Identity factoryID;
      factoryID.name = "ComponentFactory";
      factoryID.category = "ComponentFactory";
      adapter->add(new cast::CASTComponentFactory(), factoryID);
      
      //add timeserver to server
      
      Identity tsID;
      tsID.name = "TimeServer";
      tsID.category = "TimeServer";
      adapter->add(new cast::CASTTimeServer(), tsID);
      
      
      adapter->activate();
#ifdef GOOGLE_PROFILER
      if (isProfiling) {
        while(!ic->isShutdown()) {
		ProfilerFlush();
	  sleep(5);
        }
      } else {
        ic->waitForShutdown();
      }
#else
      ic->waitForShutdown();
#endif
      return 0; 
    }

  };

}; 
  
int 
main(int argc, char* argv[]) {
  cast::ComponentServer app; 
  try {
    return app.main(argc, argv ); 
  }
  catch(...) {
    cout<<"Exception in ComponentServer"<<endl;
    return 1;
  }
}
