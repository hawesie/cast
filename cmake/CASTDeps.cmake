# Find the things that cast requires, and complain if they're not there

find_package(Boost COMPONENTS thread)
if(NOT Boost_FOUND)
	message( FATAL_ERROR "Boost not found. Please install it before installing CAST." )
endif(NOT Boost_FOUND)


find_package(Ice)
if(NOT ICE_FOUND)
       message( FATAL_ERROR "Ice not found. Please install it before installing CAST." )
endif(NOT ICE_FOUND)

find_package(Log4CXX)
if(NOT LOG4CXX_FOUND)
       message( FATAL_ERROR "log4cxx not found. Please install it before installing CAST." )
endif(NOT LOG4CXX_FOUND)


include( UseIce )
include( UseBoost )
include( UseLog4CXX )



