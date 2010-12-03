#include "PatternConverters.hpp"
#include <cast/core/ComponentLogger.hpp>
#include <cast/core/CASTUtils.hpp>

#include <log4cxx/level.h>

#include <iostream>

using namespace std;
using namespace cast::cdl;
using namespace cast::core::logging;
using namespace log4cxx;

IMPLEMENT_LOG4CXX_OBJECT(ComponentIDPatternConverter)
IMPLEMENT_LOG4CXX_OBJECT(SubarchitectureIDPatternConverter)
IMPLEMENT_LOG4CXX_OBJECT(CASTTimePatternConverter)
IMPLEMENT_LOG4CXX_OBJECT(RetroLevelPatternConverter)
IMPLEMENT_LOG4CXX_OBJECT(ColourStartPatternConverter)
IMPLEMENT_LOG4CXX_OBJECT(ColourEndPatternConverter)


namespace cast
{
  //super namespacing to match java declaration and thus config file details
  namespace core
  {
    
    namespace logging
    {
      
      
      ComponentIDPatternConverter::ComponentIDPatternConverter() :
	LoggingEventPatternConverter(LOG4CXX_STR("ComponentIDPatternConverter"),
				     LOG4CXX_STR("ComponentIDPatternConverter")) {

      }

      void 
      ComponentIDPatternConverter::format(const log4cxx::spi::LoggingEventPtr& event,
					  LogString& toAppendTo,
					  log4cxx::helpers::Pool& p) const {	
	CASTLoggingEventPtr ce = event;
	if(ce != NULL) {
	  toAppendTo.append(ce->getAdditions().getComponentID());
	}	
	else {//don't do anything here if we don't have the info
	  // 	  std::cout<<"no cast do\n";
 	}

      }
      




      log4cxx::pattern::PatternConverterPtr 
      ComponentIDPatternConverter::newInstance( const std::vector<LogString> & options) {	
	static pattern::PatternConverterPtr def(new ComponentIDPatternConverter());
	return (def);
      }







      SubarchitectureIDPatternConverter::SubarchitectureIDPatternConverter() :
	LoggingEventPatternConverter(LOG4CXX_STR("SubarchitectureIDPatternConverter"),
				     LOG4CXX_STR("SubarchitectureIDPatternConverter")) {

      }

      void 
      SubarchitectureIDPatternConverter::format(const log4cxx::spi::LoggingEventPtr& event,
					  LogString& toAppendTo,
					  log4cxx::helpers::Pool& p) const {	
	CASTLoggingEventPtr ce = event;
	if(ce != NULL) {
	  toAppendTo.append(ce->getAdditions().getSubarchitectureID());
	}	
	else {//don't do anything here if we don't have the info
	  // 	  std::cout<<"no cast do\n";
 	}

      }

      
      log4cxx::pattern::PatternConverterPtr 
      SubarchitectureIDPatternConverter::newInstance( const std::vector<LogString> & options) {	
	static pattern::PatternConverterPtr def(new SubarchitectureIDPatternConverter());
	return (def);
      }









      
      CASTTimePatternConverter::CASTTimePatternConverter() :
	LoggingEventPatternConverter(LOG4CXX_STR("CASTTimePatternConverter"),
				     LOG4CXX_STR("CASTTimePatternConverter"))
	 {

      }

      void 
      CASTTimePatternConverter::format(const log4cxx::spi::LoggingEventPtr& event,
					  LogString& toAppendTo,
					  log4cxx::helpers::Pool& p) const {	

	if(!m_timeServer) {
	  m_timeServer = cast::getTimeServer();
	}
	
	CASTTime time(m_timeServer->getCASTTime());
	std::ostringstream formattedTime;
	formattedTime<<time.s<<":"<<time.us;

	toAppendTo.append(formattedTime.str());
      }
      




      log4cxx::pattern::PatternConverterPtr 
      CASTTimePatternConverter::newInstance( const std::vector<LogString> & options) {	
	static pattern::PatternConverterPtr def(new CASTTimePatternConverter());
	return (def);
      }




      RetroLevelPatternConverter::RetroLevelPatternConverter() :
	LoggingEventPatternConverter(LOG4CXX_STR("RetroLevelPatternConverter"),
				     LOG4CXX_STR("RetroLevelPatternConverter")) {
	
      }

      void 
      RetroLevelPatternConverter::format(const log4cxx::spi::LoggingEventPtr& event,
					  LogString& toAppendTo,
					  log4cxx::helpers::Pool& p) const {	
	
	switch (event->getLevel()->toInt()) {
	case Level::INFO_INT:
	  toAppendTo.append("");
	  return;
	case Level::TRACE_INT:
	  toAppendTo.append("DEBUG ");
	  return;	
	case Level::DEBUG_INT:
	  toAppendTo.append("LOG ");
	  return;
	default:
	  return;
	}
      }
      




      log4cxx::pattern::PatternConverterPtr 
      RetroLevelPatternConverter::newInstance( const std::vector<LogString> & options) {	
	static pattern::PatternConverterPtr def(new RetroLevelPatternConverter());
	return (def);
      }








      ColourStartPatternConverter::ColourStartPatternConverter() :
	LoggingEventPatternConverter(LOG4CXX_STR("ColourStartPatternConverter"),
				     LOG4CXX_STR("ColourStartPatternConverter")) {

      }

      void 
      ColourStartPatternConverter::format(const log4cxx::spi::LoggingEventPtr& event,
					  LogString& toAppendTo,
					  log4cxx::helpers::Pool& p) const {	
	CASTLoggingEventPtr ce = event;
	if(ce != NULL) {
	  toAppendTo.append(ce->getAdditions().getColourStart());
	}	
	else {//don't do anything here if we don't have the info
	  // 	  std::cout<<"no cast do\n";
 	}

      }
      
      log4cxx::pattern::PatternConverterPtr 
      ColourStartPatternConverter::newInstance( const std::vector<LogString> & options) {	
	static pattern::PatternConverterPtr def(new ColourStartPatternConverter());
	return (def);
      }







      ColourEndPatternConverter::ColourEndPatternConverter() :
	LoggingEventPatternConverter(LOG4CXX_STR("ColourEndPatternConverter"),
				     LOG4CXX_STR("ColourEndPatternConverter")) {

      }

      void 
      ColourEndPatternConverter::format(const log4cxx::spi::LoggingEventPtr& event,
					  LogString& toAppendTo,
					  log4cxx::helpers::Pool& p) const {	
	CASTLoggingEventPtr ce = event;
	if(ce != NULL) {
	  toAppendTo.append("\033[0m");
	}	
	else {//don't do anything here if we don't have the info
	  // 	  std::cout<<"no cast do\n";
 	}

      }
      
      log4cxx::pattern::PatternConverterPtr 
      ColourEndPatternConverter::newInstance( const std::vector<LogString> & options) {	
	static pattern::PatternConverterPtr def(new ColourEndPatternConverter());
	return (def);
      }

    }  // namespace logging
  } // namespace core
} // namespace cast

