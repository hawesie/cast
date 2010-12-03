include(${CAST_ROOT}/cmake/Slice2Cpp.cmake)

set(slice_source CDL.ice)

cast_slice2cpp(GEN_CPP GEN_HPP ${slice_source})

add_library(CDL SHARED ${GEN_CPP} ${GEN_HPP})

install(TARGETS CDL LIBRARY DESTINATION lib/cast ARCHIVE DESTINATION lib/cast)
install(FILES ${GEN_HPP} DESTINATION include/cast/slice)
install(FILES ${slice_source} DESTINATION share/cast/slice)

set( slice2py_command  	      ${ICE_HOME}/bin/slice2py${EXE_EXTENSION} )
set( slice2py_sources         ${CAST_ROOT}/src)
set( slice2py_prefix         python)
set( slice2py_output         ${slice2py_sources}/${slice2py_prefix})


add_custom_command(TARGET CDL
		   POST_BUILD	
		   DEPENDS CDL
		   COMMAND ${slice2py_command} --output-dir ${slice2py_output} ${CAST_ROOT}/src/slice/CDL.ice
		   COMMENT "Generating Python source"
		   )

set( slice2py_results ${CAST_ROOT}/src/python/cast/__init__.py
${CAST_ROOT}/src/python/cast/cdl/__init__.py
${CAST_ROOT}/src/python/cast/cdl/testing/__init__.py
${CAST_ROOT}/src/python/cast/examples/__init__.py
${CAST_ROOT}/src/python/cast/examples/autogen/__init__.py
${CAST_ROOT}/src/python/cast/interfaces/__init__.py)


install(FILES ${slice2py_results} DESTINATION lib/cast)
