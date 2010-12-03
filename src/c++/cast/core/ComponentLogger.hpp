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




//stolen and edited from logger.h from log4cxx


#if !defined(LOG4CXX_UNLIKELY)
#if __GNUC__ >= 3
/**
Provides optimization hint to the compiler
to optimize for the expression being false.
@param expr boolean expression.
@returns value of expression.
*/
#define LOG4CXX_UNLIKELY(expr) __builtin_expect(expr, 0)
#else
/**
Provides optimization hint to the compiler
to optimize for the expression being false.
@param expr boolean expression.
@returns value of expression.
**/
#define LOG4CXX_UNLIKELY(expr) expr
#endif
#endif


/**
Logs a message to a specified logger with a specified level.

@param logger the logger to be used.
@param level the level to log.
@param message the message string to log.
*/
#define CAST_LOG(logger, level, message, additions) {	\
        if (logger->isEnabledFor(level)) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
	  logger->forcedLog(level, oss_.str(oss_ << message), LOG4CXX_LOCATION, additions); } }

/**
Logs a message to a specified logger with a specified level.

@param logger the logger to be used.
@param level the level to log.
@param message the message string to log in the internal encoding.
*/
#define CAST_LOGLS(logger, level, message, additions) {	\
        if (logger->isEnabledFor(level)) {\
           ::log4cxx::helpers::LogCharMessageBuffer oss_; \
           logger->forcedLog(level, oss_.str(oss_ << message), LOG4CXX_LOCATION, additions); } }

/**
Logs a message to a specified logger with the DEBUG level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define CAST_DEBUG(logger, message, additions) { \
        if (LOG4CXX_UNLIKELY(logger->isDebugEnabled())) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getDebug(), oss_.str(oss_ << message), LOG4CXX_LOCATION, additions); }}

/**
Logs a message to a specified logger with the TRACE level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define CAST_TRACE(logger, message, additions) { \
        if (LOG4CXX_UNLIKELY(logger->isTraceEnabled())) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getTrace(), oss_.str(oss_ << message), LOG4CXX_LOCATION, additions); }}


/**
Logs a message to a specified logger with the INFO level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define CAST_INFO(logger, message, additions) { \
        if (logger->isInfoEnabled()) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getInfo(), oss_.str(oss_ << message), LOG4CXX_LOCATION, additions); }}

/**
Logs a message to a specified logger with the WARN level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define CAST_WARN(logger, message, additions) { \
        if (logger->isWarnEnabled()) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getWarn(), oss_.str(oss_ << message), LOG4CXX_LOCATION, additions); }}

/**
Logs a message to a specified logger with the ERROR level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define CAST_ERROR(logger, message, additions) { \
        if (logger->isErrorEnabled()) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getError(), oss_.str(oss_ << message), LOG4CXX_LOCATION, additions); }}

/**
Logs a error if the condition is not true.

@param logger the logger to be used.
@param condition condition
@param message the message string to log.
*/
#define CAST_ASSERT(logger, condition, message, additions) {	\
        if (!(condition) && logger->isErrorEnabled()) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getError(), oss_.str(oss_ << message), LOG4CXX_LOCATION, additions); }}


/**
Logs a message to a specified logger with the FATAL level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define CAST_FATAL(logger, message, additions) { \
        if (logger->isFatalEnabled()) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getFatal(), oss_.str(oss_ << message), LOG4CXX_LOCATION, additions); }}




#endif //CAST_COMPONENT_LOGGER_HPP
