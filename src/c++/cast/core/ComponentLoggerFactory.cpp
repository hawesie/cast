#include "ComponentLoggerFactory.hpp"
#include "ComponentLogger.hpp"


using namespace log4cxx;
using namespace log4cxx::helpers;
using namespace log4cxx::spi;

using namespace cast;
using namespace cast::core::logging;

IMPLEMENT_LOG4CXX_OBJECT(ComponentLoggerFactory)

namespace cast {
namespace core {
namespace logging {

  LoggerPtr 
  ComponentLoggerFactory::makeNewLoggerInstance(log4cxx::helpers::Pool& pool, 
						const LogString& name) const {
    return new ComponentLogger(pool,name);
  }

}
}
}
