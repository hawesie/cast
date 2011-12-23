cmake_minimum_required(VERSION 2.8)

project (LuaJavaBinaries)

if(NOT TARGET "clean-all")
   #message(" *** adding clean-all ")
   add_custom_target(clean-all)
endif()

find_package(JNI)

if("${APPLE}" AND ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   set(LUATARGET macosx)
   set(JDKROOT "/usr") # Strange config in LuaJava Makefile
elseif(${UNIX})
   set(LUATARGET linux)
   set(JDKROOT "/usr") # Strange config in LuaJava Makefile
else()
   message("LuaJavaBinaries: Unsupported platform.")
   return()
endif()

add_custom_target (LuaLib)
add_custom_command (
   TARGET  LuaLib
   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/lua-5.1.4
   COMMAND make ${LUATARGET}
)
add_library(lua STATIC IMPORTED)
set_property(TARGET lua
   PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/lua-5.1.4/src/liblua.a
   )

add_custom_target (clean-LuaLib
   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/lua-5.1.4
   COMMAND make clean
   )

add_custom_target (LuaJavaJar ALL
   DEPENDS LuaLib
   )
add_custom_command (
   TARGET  LuaJavaJar
   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/luajava-1.1
   COMMAND make JDK=${JDKROOT} 
   )

add_custom_target (clean-LuaJavaJar
   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/luajava-1.1
   COMMAND make clean
   )

add_dependencies(clean-all
   DEPENDS clean-LuaLib clean-LuaJavaJar
   )

set (LJVERSION "1.1")
set (LUAJAVATARGET "luajava-${LJVERSION}")
include_directories(lua-5.1.4/src ${JAVA_INCLUDE_PATH})
#link_directories(lua-5.1.4/src ${JAVA_INCLUDE_PATH})
file(GLOB LUAFILES lua-5.1.4/src/*.c)
add_library(${LUAJAVATARGET} SHARED
   luajava-${LJVERSION}/src/c/luajava.c
   )
target_link_libraries(${LUAJAVATARGET} lua)
