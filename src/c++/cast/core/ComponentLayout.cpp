/**
 * With many thanks to http://rodolfotech.blogspot.com/2009/03/adding-new-fields-to-log4cxx-logger.html
 */

#include "ComponentLayout.hpp"
#include "PatternConverters.hpp"

#include <cast/slice/CDL.hpp>

#include <iostream>

using namespace std;

using namespace log4cxx;
using namespace log4cxx::helpers;
using namespace log4cxx::spi;
using namespace log4cxx::pattern;

using namespace cast;
using namespace cdl;
using namespace cast::core::logging;


IMPLEMENT_LOG4CXX_OBJECT(ComponentLayout)

namespace cast {
namespace core {
namespace logging {

  ComponentLayout::ComponentLayout() 
    : PatternLayout(RETROCONVERSIONPATTERN)
  { 
  }
  
  ComponentLayout::ComponentLayout(const LogString& pattern)
    : PatternLayout(pattern) {
  }
  

  void 
  ComponentLayout::format(LogString& output,
			  const spi::LoggingEventPtr& event,
			  Pool& pool) const {
    PatternLayout::format(output,event,pool);
  }
  

#define RULES_PUT(spec, cls) \
specs.insert(PatternMap::value_type(LogString(LOG4CXX_STR(spec)), (PatternConstructor) cls ::newInstance))


  log4cxx::pattern::PatternMap ComponentLayout::getFormatSpecifiers() {

    //first construct with standard specs
    PatternMap specs = PatternLayout::getFormatSpecifiers();

    RULES_PUT("i", ComponentIDPatternConverter);
    RULES_PUT("componentid", ComponentIDPatternConverter);

    RULES_PUT("s", SubarchitectureIDPatternConverter);
    RULES_PUT("subarchitectureid", SubarchitectureIDPatternConverter);

    RULES_PUT("P", RetroLevelPatternConverter);
    RULES_PUT("retrolevel", RetroLevelPatternConverter);

    RULES_PUT("a", CASTTimePatternConverter);
    RULES_PUT("casttime", CASTTimePatternConverter);

    RULES_PUT("S", ColourStartPatternConverter);
    RULES_PUT("colourstart", ColourStartPatternConverter);


    RULES_PUT("E", ColourEndPatternConverter);
    RULES_PUT("colourend", ColourEndPatternConverter);

    return specs;    
  }

}
}
}
