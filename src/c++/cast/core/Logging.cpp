
#include "Logging.hpp"

#include <ComponentLayout.hpp> 
#include <CDL.hpp> 

#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/file.h>
#include <log4cxx/helpers/pool.h>
#include <log4cxx/helpers/loglog.h>
#include <log4cxx/helpers/properties.h>
#include <log4cxx/stream.h>

using namespace std;
using namespace log4cxx;
using namespace log4cxx::helpers;

namespace cast {


  using namespace cdl;

  //super namespacing to match java declaration and thus config file details
  namespace core
  {
    
  
  namespace logging {


    bool configureFromFile(const std::string & _filename) {
      File propertiesFile(_filename);
      Pool p;
      if (propertiesFile.exists(p)) {
	//PropertyConfigurator::configure(propertiesFile);
	PropertyConfigurator::configure(_filename);
	return true;
      }
      else {
	return false;
      }
    }
    
    bool configureFromEnvironment() {      
      char * propertiesFileString = getenv(LOGGINGENVVAR.c_str());    
      if (propertiesFileString != NULL) {
 	return configureFromFile(propertiesFileString);
      }      
      return false;
    }
    

    bool configureFromCWD() {
      string propertiesFileString("./");
      propertiesFileString += LOGGINGPROPERTIESFILE;
      return configureFromFile(propertiesFileString);      
    }


    bool configureFromHome() {
      char * homeDir = getenv("HOME");    
      assert(homeDir);
      string propertiesFileString(homeDir);
      propertiesFileString += "/.";
      propertiesFileString += LOGGINGPROPERTIESFILE;
      return configureFromFile(propertiesFileString);      
    }


    void configureDefault() {
      log4cxx::helpers::Properties prop;
      prop.setProperty("log4j.rootLogger", "INFO, stdout");
      prop.setProperty("log4j.appender.stdout", "org.apache.log4j.ConsoleAppender");
      prop.setProperty("log4j.appender.stdout.layout", "cast.core.logging.ComponentLayout");
      prop.setProperty("log4j.appender.stdout.layout.ConversionPattern",RETROCONVERSIONPATTERN);
      PropertyConfigurator::configure(prop);
    }

    /**
     * Rules for finding the file to use for configuring logging properties.
     * 
     * 1. Use the file indicated by the environment CAST_LOGGING_PROPERTIES
     * 
     * 2. If the env var or the file it specifies does not exist, use
     * log4j.properties in the cwd.
     * 
     * 3. If that file does not exist, use ~/.log4j.properties instead.
     * 
     * 4. And if that fails, fall back to default (stdout, ComponentLayout)
     * config.
     * 
     */
    void initLogging() {

      LogLog::setInternalDebugging(false);
      LogLog::setQuietMode(true);

      ComponentLayout::registerClass();

      // get the file specified by the env var
       if (configureFromEnvironment()) {
 	LogLog::setInternalDebugging(true);
 	LogLog::setQuietMode(false);
 	return;
       } else if (configureFromCWD()) {
	 LogLog::setInternalDebugging(true);
	 LogLog::setQuietMode(false);
	 return;	
       } else if (configureFromHome()) {
	 LogLog::setInternalDebugging(true);
	 LogLog::setQuietMode(false);
	 return;
       } else {	 
	 configureDefault();
	 LogLog::setInternalDebugging(true);
	 LogLog::setQuietMode(false);
       }
    
    }
  
}
}
}
