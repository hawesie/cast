#ifndef CAST_COMPONENT_LOGGER_HPP
#define CAST_COMPONENT_LOGGER_HPP

#include <log4cxx/logger.h>
#include <log4cxx/spi/loggingevent.h>

#include <cast/core/ComponentLoggerFactory.hpp>

namespace cast
{
  //super namespacing to match java declaration and thus config file details
  namespace core
  {
    
    namespace logging
    {

      using namespace log4cxx;

      
      /**
       * Class to hold the extra information a log event for CAST could have.
       * 
       * @author nah
       * 
       */
      class LogAdditions {

      private: 
	std::string m_id;
	std::string m_saID;
	std::string m_colourStart;
	
      public:
	LogAdditions();

	LogAdditions(const std::string & _id,
		     const std::string & _saID,
		     const std::string & _colourStart);

	LogAdditions(const LogAdditions & la);
	
	LogAdditions& operator=(const LogAdditions& la);


	const std::string & getComponentID() const {
	  return m_id;
	}

	const std::string & getSubarchitectureID() const {
	  return m_saID;
	}

	const std::string & getColourStart() const {
	  return m_colourStart;
	}
	
      };

      /**
       * Extension of LoggingEvent to include the additions.
       */
      class CASTLoggingEvent: public log4cxx::spi::LoggingEvent {
      public:
	public:
	DECLARE_LOG4CXX_OBJECT(CASTLoggingEvent)
	BEGIN_LOG4CXX_CAST_MAP()
	  LOG4CXX_CAST_ENTRY(CASTLoggingEvent)
	  LOG4CXX_CAST_ENTRY_CHAIN(LoggingEvent)
	END_LOG4CXX_CAST_MAP()
	
	/** For serialization only
	 */
	CASTLoggingEvent() {}
	
	/**
	   Instantiate a LoggingEvent from the supplied parameters.
	   
	   <p>Except timeStamp all the other fields of
	   <code>LoggingEvent</code> are filled when actually needed.
	   <p>
	   @param logger The logger of this event.
	   @param level The level of this event.
	   @param message  The message of this event.
	   @param location location of logging request.
	*/
	CASTLoggingEvent(const LogString& logger,
			 const LevelPtr& level,   const LogString& message,
			 const log4cxx::spi::LocationInfo& location, const LogAdditions & _additions) :
	  LoggingEvent(logger,level,message,location), m_additions(_additions) {}
	
	~CASTLoggingEvent() {}
	
	const LogAdditions & getAdditions() const {
	  return m_additions;
	}

      private:
	LogAdditions m_additions;
	
      };
      
      LOG4CXX_PTR_DEF(CASTLoggingEvent);
      
      class ComponentLogger : public log4cxx::Logger
      {

      protected:
	friend class ComponentLoggerFactory;

        ComponentLogger(log4cxx::helpers::Pool& pool, const LogString& name);
	
      public:
	DECLARE_ABSTRACT_LOG4CXX_OBJECT(ComponentLogger)
	BEGIN_LOG4CXX_CAST_MAP()
	  LOG4CXX_CAST_ENTRY(ComponentLogger)
	  LOG4CXX_CAST_ENTRY_CHAIN(Logger)
	END_LOG4CXX_CAST_MAP()
	  
	LOG4CXX_PTR_DEF(ComponentLogger);
	
	static LoggerPtr getLogger(const std::string& name);

	/**
	 * This method creates a new logging event and logs the event
	 * without further checks.
	 * 
	 *  @param level the level to log.
	 *  @param message message.
	 *  @param location location of source of logging request.
	 * @param _additions Extra info added to the log event
        */
        void forcedLog(const LevelPtr& level, const std::string& message,
				    const log4cxx::spi::LocationInfo& location, 
				    const LogAdditions & _additions) const;



      private:
	
	static helpers::ObjectPtrT<ComponentLoggerFactory> m_factory;

      };
    }  // namespace logging
  } // namespace core
} // namespace cast


#endif //CAST_COMPONENT_LOGGER_HPP
