
include_directories(${ICE_HOME}/include)
link_directories(${ICE_HOME}/lib)

#workaround for macports stupid renaming
if(EXISTS "${ICE_HOME}/lib/libZeroCIce.dylib")
  set( ICE_LIBS ZeroCIce IceUtil )
else(EXISTS "${ICE_HOME}/lib/libZeroCIce.dylib")
  set( ICE_LIBS Ice IceUtil )
endif(EXISTS "${ICE_HOME}/lib/libZeroCIce.dylib")

link_libraries( ${ICE_LIBS} )
