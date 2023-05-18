cmake_minimum_required(VERSION 3.13)
project(clark)

set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # disables warning about DLL export 
  # this is annoying when using C++ STL
  add_compile_options(/wd4251)  
endif()

set(CLARK_PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR})

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if(WIN32)
  list(APPEND CMAKE_PREFIX_PATH "C:/usr")
  list(APPEND CMAKE_LIBRARY_PATH "C:/usr/bin")
  list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/build/usr")
  list(APPEND CMAKE_LIBRARY_PATH "${CMAKE_CURRENT_LIST_DIR}/build/usr/bin")
endif()

##################################################################
###### install config
##################################################################

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

##################################################################
###### C++17 clang (Travis CI)
##################################################################

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  message("Building with clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

##################################################################
###### Qt
##################################################################

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 COMPONENTS Core Xml Gui Network Widgets Concurrent)

##################################################################
###### dependencies
##################################################################

add_subdirectory(dependencies)

##################################################################
###### modules
##################################################################

add_subdirectory(modules)

##################################################################
###### clark
##################################################################

add_subdirectory(src)

##################################################################
###### install
##################################################################

if (WIN32)
  configure_file("scripts/make-release.bat.in" "${CMAKE_BINARY_DIR}/make-release.bat")
endif()

