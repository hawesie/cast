#ifndef CAST_COMPONENT_LAYOUT_HPP
#define CAST_COMPONENT_LAYOUT_HPP

#include <log4cxx/patternlayout.h>
#include <log4cxx/helpers/pool.h>

namespace cast
{
  //super namespacing to match java declaration and thus config file details
  namespace core
  {
    
    namespace logging
    {
      
      using namespace log4cxx;
      
      class ComponentLayout : public log4cxx::PatternLayout
      {
	
      public:
	DECLARE_LOG4CXX_OBJECT(ComponentLayout)
	  BEGIN_LOG4CXX_CAST_MAP()
	  LOG4CXX_CAST_ENTRY(ComponentLayout)
	  LOG4CXX_CAST_ENTRY_CHAIN(PatternLayout)
	  END_LOG4CXX_CAST_MAP()
	  
	  /**
	   * Does nothing
	   */
	  ComponentLayout();
	
	/**
	 * Constructs a PatternLayout using the supplied conversion pattern.
	 */
	ComponentLayout(const LogString& pattern);
	
	
	void format(LogString & output,
		    const spi::LoggingEventPtr & event,
		    helpers::Pool & pool) const;
	

	virtual 
	log4cxx::pattern::PatternMap getFormatSpecifiers();

	LOG4CXX_PTR_DEF(ComponentLayout);
      };
    }  // namespace logging
  } // namespace core
} // namespace cast


#endif //CAST_COMPONENT_LAYOUT_HPP
