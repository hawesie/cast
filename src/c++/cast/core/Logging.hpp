#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/file.h>
#include <log4cxx/helpers/pool.h>
#include <log4cxx/helpers/loglog.h>
#include <log4cxx/helpers/properties.h>
#include <log4cxx/stream.h>


namespace cast {
//super namespacing to match java declaration and thus config file details
namespace core {
namespace logging {

  bool configureFromFile(const std::string & _filename);

  bool configureFromEnvironment();
  
  bool configureFromCWD();

  bool configureFromHome();

  void configureDefault();

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
  void initLogging();
}
}
}
