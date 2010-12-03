set(sources CASTUtils.cpp ComponentLogger.cpp
 ComponentLoggerFactory.cpp PatternConverters.cpp ComponentLayout.cpp
 CASTComponent.cpp SubarchitectureComponent.cpp
 CASTComponentPermissionsMap.cpp CASTWorkingMemory.cpp
 CASTWMPermissionsMap.cpp CASTTimer.cpp Logging.cpp)

set(headers CASTUtils.hpp ComponentLogger.hpp
 ComponentLoggerFactory.hpp PatternConverters.hpp ComponentLayout.hpp
 CASTComponent.hpp SubarchitectureComponent.hpp
 CASTComponentPermissionsMap.hpp CASTWorkingMemory.hpp
 CASTWMPermissionsMap.hpp CASTData.hpp CASTWorkingMemoryInterface.hpp
 StringMap.hpp CASTTimer.hpp Logging.hpp)


add_library(CASTCore SHARED ${sources} ${headers})

target_link_libraries(CASTCore CDL)

install(TARGETS CASTCore LIBRARY DESTINATION lib/cast ARCHIVE DESTINATION lib/cast)
install(FILES ${headers} DESTINATION include/cast/core)
