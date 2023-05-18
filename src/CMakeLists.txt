

# Window application:
# See: https://stackoverflow.com/questions/33873735/correctly-set-visual-studio-linker-flag-subsystem-in-cmake
add_executable(clark WIN32 "main.cpp")

target_include_directories(clark PRIVATE "${PROJECT_SOURCE_DIR}/src")

target_link_libraries(clark clark-app)

set_target_properties(clark PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(clark PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

if (WIN32)
  set_target_properties(clark PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=${Qt5_DIR}/../../../bin;${TINYXML2_INCLUDE}/../bin;%PATH%")
endif()

foreach(_source IN ITEMS ${HDR_FILES} ${SRC_FILES} ${QRC_FILES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()
