
##################################################################
###### dynlib
##################################################################

set(DYNLIB_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dynlib" CACHE PATH "Provide the directory of the dynlib library")

if ("${DYNLIB_PROJECT_DIR}" STREQUAL "")
  message(FATAL_ERROR "The directory of the dynlib project must be specified")  
endif()

file(GLOB_RECURSE DYNLIB_SRC_FILES ${DYNLIB_PROJECT_DIR}/src/*.cpp)
file(GLOB_RECURSE DYNLIB_HDR_FILES 
  ${DYNLIB_PROJECT_DIR}/include/*.h
  ${DYNLIB_PROJECT_DIR}/src/*.h
)
add_library(dynlib STATIC ${DYNLIB_HDR_FILES} ${DYNLIB_SRC_FILES})
target_include_directories(dynlib PUBLIC "${DYNLIB_PROJECT_DIR}/include")
target_compile_definitions(dynlib PUBLIC -DDYNLIB_STATIC_LINKING)

if (NOT WIN32)
  target_link_libraries(dynlib ${CMAKE_DL_LIBS})
  set_property(TARGET dynlib PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

foreach(_source IN ITEMS ${DYNLIB_HDR_FILES} ${DYNLIB_SRC_FILES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

##################################################################
###### cpptok
##################################################################

set(BUILD_CPPTOK_TESTS OFF CACHE BOOL "whether to build cpptok tests")

add_subdirectory(cpptok)

set(CPPTOK_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cpptok" CACHE PATH "Provide the directory of the cpptok library")

if ("${CPPTOK_PROJECT_DIR}" STREQUAL "")
  message(FATAL_ERROR "The directory of the cpptok project must be specified")  
endif()

##################################################################
###### libclang-utils
##################################################################

set(BUILD_LIBCLANGUTILS_TESTS OFF CACHE BOOL "whether to build libclang-utils tests")

add_subdirectory(libclang-utils)

set(LIBCLANGUTILS_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libclang-utils" CACHE PATH "Provide the directory of the libclang-utils library")

if ("${LIBCLANGUTILS_PROJECT_DIR}" STREQUAL "")
  message(FATAL_ERROR "The directory of the libclang-utils project must be specified")  
endif()

##################################################################
###### vcxproj
##################################################################

add_subdirectory(vcxproj)
