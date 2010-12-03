project(PythonCAST)

set(CPP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../c++)
set(CPP_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/../c++)

set(INCLUDE_DIRS
   ${CPP_SRC_DIR}
   ${ICE_HOME}/include
   )
set(LIBRARY_DIRS
   ${CPP_BIN_DIR}/cast/architecture
   ${CPP_BIN_DIR}/cast/core
   ${CPP_BIN_DIR}/cast/slice
   # ${CMAKE_INSTALL_PREFIX}/lib/cast
   )

if(NOT "${LOG4CXX_LIBRARY_DIRS}" STREQUAL "")
   set(LIBRARY_DIRS ${LIBRARY_DIRS} ${LOG4CXX_LIBRARY_DIRS})
   set(INCLUDE_DIRS ${INCLUDE_DIRS} ${LOG4CXX_INCLUDE_DIRS})
endif(NOT "${LOG4CXX_LIBRARY_DIRS}" STREQUAL "")

foreach(idir ${INCLUDE_DIRS})
   if(NOT "${idir}" STREQUAL "")
      set(INCLUDE_DIRS_LIST "'${idir}', ${INCLUDE_DIRS_LIST}")
   endif(NOT "${idir}" STREQUAL "")
endforeach(idir)

foreach(idir ${ICE_LIBS})
   if(NOT "${idir}" STREQUAL "")
      set(ICE_LIBS_LIST "'${idir}', ${ICE_LIBS_LIST}")
   endif(NOT "${idir}" STREQUAL "")
endforeach(idir)

foreach(idir ${LIBRARY_DIRS})
   if(NOT "${idir}" STREQUAL "")
      set(LIBRARY_DIRS_LIST "'${idir}', ${LIBRARY_DIRS_LIST}")
   endif(NOT "${idir}" STREQUAL "")
endforeach(idir)

set(SETUP_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(MKSETUP_PY ${SETUP_DIR}/mksetup.py)
set(SETUP_PY ${SETUP_DIR}/setup.py)

set(PACKAGE_DIR ${CMAKE_CURRENT_SOURCE_DIR}) 
set(EXT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}) 

configure_file(mksetup.py.in ${MKSETUP_PY} ESCAPE_QUOTES @ONLY)

add_custom_command(OUTPUT ${SETUP_PY}
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in ${MKSETUP_PY}
   COMMAND python ${MKSETUP_PY} ${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in ${SETUP_PY}
   COMMENT "generating setup.py"
   )

add_custom_target(PythonCAST ALL
  DEPENDS ${SETUP_PY} CASTCore
  COMMAND python setup.py build
  WORKING_DIRECTORY ${SETUP_DIR}
  COMMENT "running setup.py build"
  VERBATIM)

install(CODE "EXECUTE_PROCESS(WORKING_DIRECTORY ${SETUP_DIR} COMMAND python setup.py install)")
