##################################################################
###### utils library
##################################################################

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cpp)
file(GLOB_RECURSE HDR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/utils/*.h)

add_library(clark-utils STATIC ${HDR_FILES} ${SRC_FILES})
target_include_directories(clark-utils PUBLIC "${PROJECT_SOURCE_DIR}/modules")
target_link_libraries(clark-utils Qt5::Core)

##################################################################
###### program library
##################################################################

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/program/*.cpp)
file(GLOB_RECURSE HDR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/program/*.h)

add_library(clark-program STATIC ${HDR_FILES} ${SRC_FILES})
target_include_directories(clark-program PUBLIC "${PROJECT_SOURCE_DIR}/modules")
target_link_libraries(clark-program vcxproj libclang-utils)
target_link_libraries(clark-program clark-utils)
target_link_libraries(clark-program Qt5::Core)

##################################################################
###### indexing library
##################################################################

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/indexing/*.cpp)
file(GLOB_RECURSE HDR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/indexing/*.h)

add_library(clark-indexing STATIC ${HDR_FILES} ${SRC_FILES})
target_include_directories(clark-indexing PUBLIC "${PROJECT_SOURCE_DIR}/modules")
target_link_libraries(clark-indexing clark-program)

##################################################################
###### codeviewer library
##################################################################

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/codeviewer/*.cpp)
file(GLOB_RECURSE HDR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/codeviewer/*.h)

add_library(clark-codeviewer STATIC ${HDR_FILES} ${SRC_FILES})
target_include_directories(clark-codeviewer PUBLIC "${PROJECT_SOURCE_DIR}/modules")
target_link_libraries(clark-codeviewer cpptok)
target_link_libraries(clark-codeviewer Qt5::Core Qt5::Widgets)


##################################################################
###### sema library
##################################################################

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/sema/*.cpp)
file(GLOB_RECURSE HDR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/sema/*.h)

add_library(clark-sema STATIC ${HDR_FILES} ${SRC_FILES})
target_include_directories(clark-sema PUBLIC "${PROJECT_SOURCE_DIR}/modules")
target_link_libraries(clark-sema clark-program clark-codeviewer)
target_link_libraries(clark-sema Qt5::Core Qt5::Widgets Qt5::Concurrent)

##################################################################
###### app
##################################################################

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/app/*.cpp)
file(GLOB_RECURSE HDR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/app/*.h)
file(GLOB_RECURSE QRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/app/*.qrc)

add_library(clark-app STATIC ${HDR_FILES} ${SRC_FILES} ${QRC_FILES})
target_include_directories(clark-app PRIVATE "${PROJECT_SOURCE_DIR}/modules/app")
target_link_libraries(clark-app cpptok vcxproj)
target_link_libraries(clark-app clark-program clark-indexing clark-sema)
target_link_libraries(clark-app Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Concurrent)
