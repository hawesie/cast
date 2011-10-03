#include "IceAppender.hpp"

#include <Ice/Communicator.h> 
#include <Ice/Initialize.h> 

#include <log4cxx/helpers/optionconverter.h>
#include <log4cxx/helpers/stringhelper.h>
#include <log4cxx/spi/loggingevent.h>
#include <log4cxx/helpers/objectoutputstream.h>
#include <log4cxx/helpers/bytearrayoutputstream.h>


#include "cast/slice/CDL.hpp"
#include "cast/core/CASTUtils.hpp"

using namespace log4cxx;
using namespace log4cxx::helpers;

using namespace cast;
using namespace cast::core::logging;
using namespace cast::interfaces;

using namespace Ice;

IMPLEMENT_LOG4CXX_OBJECT(IceAppender)

namespace cast {
  namespace core {
    namespace logging {
      
      IceAppender::IceAppender() : 
        m_logHost(),
        m_logPort(cast::cdl::LOGGINGPORT) {
        LogLog::setInternalDebugging(true);
      }
      
      IceAppender::~IceAppender() {
        finalize();
      }
      
      void IceAppender::append(const spi::LoggingEventPtr& event, log4cxx::helpers::Pool& pool) {
        LogLog::debug(LOG4CXX_STR("message: " + event->getMessage()));
//        m_logServer->logMessage(event->getMessage());

        
        // TODO check efficiency concerns about doing this
        // every time

        //Ptr is reference counted, so no manual deletions pleaase...
        ByteArrayOutputStreamPtr os = new ByteArrayOutputStream();
        ObjectOutputStreamPtr oos = new ObjectOutputStream(os,pool);
        
        //this block copied from socketappender.h so I guess it might do something important...
        LogString ndcVal;
        event->getNDC(ndcVal);
        event->getThreadName();
        // Get a copy of this thread's MDC.
        event->getMDCCopy();
        
        try {
          event->write(*oos, pool);
          oos->flush(pool);
          m_logServer->logSerialisedEvent(os->toByteArray());
        } catch(std::exception& e) {
          LogLog::warn(LOG4CXX_STR("Problem dispatching serialised log event: "), e);
        }

        
      }

      void IceAppender::setOption(const LogString& option, const LogString& value) {
        if (StringHelper::equalsIgnoreCase(option, LOG4CXX_STR("HOST"), LOG4CXX_STR("host"))) {
          setHost(value);
            LogLog::debug(LOG4CXX_STR("setHost: " + value));
        }
        else if (StringHelper::equalsIgnoreCase(option, LOG4CXX_STR("PORT"), LOG4CXX_STR("port"))) {
          setPort(OptionConverter::toInt(value, m_logPort));
          LogLog::debug(LOG4CXX_STR("setPort: " + value));

        }
        else {
          AppenderSkeleton::setOption(option, value);
        }
      }

      void IceAppender::activateOptions(log4cxx::helpers::Pool& _pool) {
       	m_logServer = getLoggingServer(m_logHost, m_logPort);        
      }

      LoggingServerPrx 
      IceAppender::getLoggingServer(const std::string & _logHost, const int & _logPort) {
        
        CommunicatorPtr ic = Ice::initialize();      
        assert(ic);
        
        Identity id;
        id.name = "CASTLogServer";
        id.category = "LoggingServer";
        
        std::ostringstream serverAddr;
        serverAddr << ic->identityToString(id)
        << ":default ";
        if(!_logHost.empty()) {
          serverAddr << " -h " << _logHost; 
          }
        serverAddr <<  " -p " << _logPort;
        
        ObjectPrx base = ic->stringToProxy(serverAddr.str());
        assert(base);
        
        LoggingServerPrx logServer = LoggingServerPrx::checkedCast(base);
        assert(logServer);
        return logServer;
      }

    }
  }
}