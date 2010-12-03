#include "ComponentLogger.hpp"

#include <cast/core/ComponentLoggerFactory.hpp>

#include <iostream>

using namespace std;

using namespace log4cxx;
using namespace log4cxx::helpers;
using namespace log4cxx::spi;

using namespace cast;
using namespace cast::core::logging;

IMPLEMENT_LOG4CXX_OBJECT(CASTLoggingEvent)
IMPLEMENT_LOG4CXX_OBJECT(ComponentLogger)

namespace cast {
namespace core {
namespace logging {

  LogAdditions::LogAdditions(): 
    m_id(""), m_saID(""), m_colourStart("") {}

  LogAdditions::LogAdditions(const std::string & _id,
			     const std::string & _saID,
			     const std::string & _colourStart): 
    m_id(_id), m_saID(_saID), m_colourStart(_colourStart) {}
  
  LogAdditions::LogAdditions(const LogAdditions & la) :
    m_id(la.m_id), m_saID(la.m_saID), m_colourStart(la.m_colourStart) {}


  LogAdditions& LogAdditions::operator=(const LogAdditions& la) {
    m_id = la.m_id;
    m_saID = la.m_saID;
    m_colourStart = la.m_colourStart;
    return *this;
  }

  helpers::ObjectPtrT<ComponentLoggerFactory> ComponentLogger::m_factory = new ComponentLoggerFactory();

  ComponentLogger::ComponentLogger(log4cxx::helpers::Pool& pool, const LogString& name) :
    Logger(pool,name) {

  }
  
  LoggerPtr 
  ComponentLogger::getLogger(const std::string& name) {
    return Logger::getLoggerLS(name,m_factory);
  }

  void 
  ComponentLogger::forcedLog(const LevelPtr& level, const std::string& message,
			     const log4cxx::spi::LocationInfo& location, 
			     const LogAdditions & _additions) const {
    Pool p;
    LOG4CXX_DECODE_CHAR(msg, message);
    CASTLoggingEventPtr event(new CASTLoggingEvent(name, level, msg, location, _additions));
    callAppenders(event, p);
  }

}
}
}
