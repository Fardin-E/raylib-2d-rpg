# CMakeList.txt : CMake project for raylib-2d-rpg, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MSVC specific settings
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_options(/arch:AVX2)
endif()

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("raylib-2d-rpg")

#Add raylib
add_subdirectory(thirdparty/raylib-5.0)

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Add source to this project's executable.
add_executable (${CMAKE_PROJECT_NAME} ${MY_SOURCES})

# Copy assets folder to the build directory
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"              # Path to assets folder in your source directory
    "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/assets"  # Destination in the build directory
)

# Link the executable with Raylib
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE raylib)

# Include directories
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET raylib-2d-rpg PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
