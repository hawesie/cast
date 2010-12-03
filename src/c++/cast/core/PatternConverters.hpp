#ifndef CAST_PATTERN_CONVERTERS_HPP
#define CAST_PATTERN_CONVERTERS_HPP

#include <cast/core/ComponentLogger.hpp>
#include <cast/slice/CDL.hpp>

#include <log4cxx/patternlayout.h>
#include <log4cxx/pattern/loggingeventpatternconverter.h>


namespace cast
{
  //super namespacing to match java declaration and thus config file details
  namespace core
  {
    
    namespace logging
    {
      
      using namespace log4cxx;
      
      class ComponentIDPatternConverter: public log4cxx::pattern::LoggingEventPatternConverter {
      protected:	
	ComponentIDPatternConverter();
      public:
	DECLARE_LOG4CXX_PATTERN(ComponentIDPatternConverter)
	BEGIN_LOG4CXX_CAST_MAP()
	LOG4CXX_CAST_ENTRY(ComponentIDPatternConverter)
	LOG4CXX_CAST_ENTRY_CHAIN(LoggingEventPatternConverter)
        END_LOG4CXX_CAST_MAP()

	virtual 
	~ComponentIDPatternConverter(){};
	
	void format(const log4cxx::spi::LoggingEventPtr& event,
		    LogString& toAppendTo,
		    log4cxx::helpers::Pool& p) const;


	static 
	log4cxx::pattern::PatternConverterPtr newInstance( const std::vector<LogString> & options);

      };


      class SubarchitectureIDPatternConverter: public log4cxx::pattern::LoggingEventPatternConverter {
      protected:	
	SubarchitectureIDPatternConverter();
      public:
	DECLARE_LOG4CXX_PATTERN(SubarchitectureIDPatternConverter)
	BEGIN_LOG4CXX_CAST_MAP()
	LOG4CXX_CAST_ENTRY(SubarchitectureIDPatternConverter)
	LOG4CXX_CAST_ENTRY_CHAIN(LoggingEventPatternConverter)
        END_LOG4CXX_CAST_MAP()

	virtual 
	~SubarchitectureIDPatternConverter(){};
	
	void format(const log4cxx::spi::LoggingEventPtr& event,
		    LogString& toAppendTo,
		    log4cxx::helpers::Pool& p) const;


	static 
	log4cxx::pattern::PatternConverterPtr newInstance( const std::vector<LogString> & options);

      };


      class CASTTimePatternConverter: public log4cxx::pattern::LoggingEventPatternConverter {

      private:
	mutable ::cast::interfaces::TimeServerPrx m_timeServer;

      protected:	
	CASTTimePatternConverter();
      public:
	DECLARE_LOG4CXX_PATTERN(CASTTimePatternConverter)
	BEGIN_LOG4CXX_CAST_MAP()
	LOG4CXX_CAST_ENTRY(CASTTimePatternConverter)
	LOG4CXX_CAST_ENTRY_CHAIN(LoggingEventPatternConverter)
        END_LOG4CXX_CAST_MAP()

	virtual 
	~CASTTimePatternConverter(){};
	
	void format(const log4cxx::spi::LoggingEventPtr& event,
		    LogString& toAppendTo,
		    log4cxx::helpers::Pool& p) const;


	static 
	log4cxx::pattern::PatternConverterPtr newInstance( const std::vector<LogString> & options);

      };



      class RetroLevelPatternConverter: public log4cxx::pattern::LoggingEventPatternConverter {

      protected:	
	RetroLevelPatternConverter();
      public:
	DECLARE_LOG4CXX_PATTERN(RetroLevelPatternConverter)
	BEGIN_LOG4CXX_CAST_MAP()
	LOG4CXX_CAST_ENTRY(RetroLevelPatternConverter)
	LOG4CXX_CAST_ENTRY_CHAIN(LoggingEventPatternConverter)
        END_LOG4CXX_CAST_MAP()

	virtual 
	~RetroLevelPatternConverter(){};
	
	void format(const log4cxx::spi::LoggingEventPtr& event,
		    LogString& toAppendTo,
		    log4cxx::helpers::Pool& p) const;


	static 
	log4cxx::pattern::PatternConverterPtr newInstance( const std::vector<LogString> & options);

      };



      class ColourStartPatternConverter: public log4cxx::pattern::LoggingEventPatternConverter {
      protected:	
	ColourStartPatternConverter();
      public:
	DECLARE_LOG4CXX_PATTERN(ColourStartPatternConverter)
	BEGIN_LOG4CXX_CAST_MAP()
	LOG4CXX_CAST_ENTRY(ColourStartPatternConverter)
	LOG4CXX_CAST_ENTRY_CHAIN(LoggingEventPatternConverter)
        END_LOG4CXX_CAST_MAP()

	virtual 
	~ColourStartPatternConverter(){};
	
	void format(const log4cxx::spi::LoggingEventPtr& event,
		    LogString& toAppendTo,
		    log4cxx::helpers::Pool& p) const;


	static 
	log4cxx::pattern::PatternConverterPtr newInstance( const std::vector<LogString> & options);

      };

      class ColourEndPatternConverter: public log4cxx::pattern::LoggingEventPatternConverter {
      protected:	
	ColourEndPatternConverter();
      public:
	DECLARE_LOG4CXX_PATTERN(ColourEndPatternConverter)
	BEGIN_LOG4CXX_CAST_MAP()
	LOG4CXX_CAST_ENTRY(ColourEndPatternConverter)
	LOG4CXX_CAST_ENTRY_CHAIN(LoggingEventPatternConverter)
        END_LOG4CXX_CAST_MAP()

	virtual 
	~ColourEndPatternConverter(){};
	
	void format(const log4cxx::spi::LoggingEventPtr& event,
		    LogString& toAppendTo,
		    log4cxx::helpers::Pool& p) const;


	static 
	log4cxx::pattern::PatternConverterPtr newInstance( const std::vector<LogString> & options);

      };

    }  // namespace logging
  } // namespace core
} // namespace cast


#endif //CAST_PATTERN_CONVERTERS_HPP
