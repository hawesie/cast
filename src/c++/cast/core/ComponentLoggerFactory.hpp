#ifndef CAST_COMPONENT_LOGGER_FACTORY_HPP
#define CAST_COMPONENT_LOGGER_FACTORY_HPP

#include <log4cxx/spi/loggerfactory.h>
#include <log4cxx/helpers/objectimpl.h>

namespace cast
{
  //super namespacing to match java declaration and thus config file details
  namespace core
  {
    
    namespace logging
    {
      
      using namespace log4cxx;
      
      class ComponentLoggerFactory : public virtual log4cxx::spi::LoggerFactory, 
				     public virtual helpers::ObjectImpl
      {

      public:
	DECLARE_ABSTRACT_LOG4CXX_OBJECT(ComponentLoggerFactory)
	BEGIN_LOG4CXX_CAST_MAP()
	  LOG4CXX_CAST_ENTRY(spi::LoggerFactory)
	END_LOG4CXX_CAST_MAP()

	virtual
	~ComponentLoggerFactory() {}

	virtual 
	LoggerPtr makeNewLoggerInstance(log4cxx::helpers::Pool& pool, 
					const LogString& name) const;

	LOG4CXX_PTR_DEF(ComponentLoggerFactory);

      };
    }  // namespace logging
  } // namespace core
} // namespace cast


#endif //CAST_COMPONENT_LOGGER_FACTORY_HPP
