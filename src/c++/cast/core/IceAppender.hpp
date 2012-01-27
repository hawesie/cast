

#include <log4cxx/helpers/class.h>
#include <log4cxx/appenderskeleton.h>
#include <log4cxx/helpers/object.h>
#include <log4cxx/helpers/loglog.h>
#include <cast/slice/CDL.hpp>

namespace cast {
  namespace core {
    namespace logging {
      using namespace log4cxx;
      
      class LOG4CXX_EXPORT IceAppender  : public log4cxx::AppenderSkeleton 
      {
      public:
        DECLARE_LOG4CXX_OBJECT(IceAppender)
        BEGIN_LOG4CXX_CAST_MAP()
        LOG4CXX_CAST_ENTRY(IceAppender)
        LOG4CXX_CAST_ENTRY_CHAIN(AppenderSkeleton)
        END_LOG4CXX_CAST_MAP()
        
        IceAppender();
        ~IceAppender();
        
        virtual void activateOptions(log4cxx::helpers::Pool& /* pool */);
        
        
        virtual bool requiresLayout() const {return false;}
        
        
        void setOption(const LogString& option,
                       const LogString& value);
        
        
        
        void setHost(const std::string & _host) {
          m_logHost = _host;      
        }
        
        void setPort(const int & _port) {
          m_logPort = _port;
        }
        
      protected:
        void append(const spi::LoggingEventPtr& event, log4cxx::helpers::Pool& pool);
        virtual void close() {}
        
      private:
        
        std::string m_logHost;
        int m_logPort;
        cast::interfaces::LoggingServerPrx m_logServer;
        
        cast::interfaces::LoggingServerPrx 
        getLoggingServer(const std::string & _logHost, const int & _logPort);
        
      };
      
      LOG4CXX_PTR_DEF(IceAppender);
      
    }
  }
}