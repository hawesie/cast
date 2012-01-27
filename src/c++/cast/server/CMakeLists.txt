



set(sources ComponentServer.cpp ComponentCreator.cpp CASTComponentFactory.cpp CASTTimeServer.cpp)

set(headers ComponentCreator.hpp CASTComponentFactory.hpp CASTTimeServer.hpp)

add_executable (cast-server-c++ ${sources} ${headers}) 

target_link_libraries(cast-server-c++ dl)
target_link_libraries(cast-server-c++ ${ICE_LIBS})
target_link_libraries(cast-server-c++ CDL CASTCore CASTArchitecture)

if(GOOGLE_PROFILER)
add_definitions(-DGOOGLE_PROFILER)
target_link_libraries(cast-server-c++ profiler)
endif(GOOGLE_PROFILER)


IF (APPLE)
   FIND_LIBRARY(COREFOUNDATION_LIBRARY CoreFoundation )
   target_link_libraries(cast-server-c++ ${COREFOUNDATION_LIBRARY})
ENDIF (APPLE)

install(TARGETS cast-server-c++ RUNTIME DESTINATION bin)
install(FILES ${headers} DESTINATION include/cast/server)

