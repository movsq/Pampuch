cmake_minimum_required(VERSION 3.20)
project(Pampuch CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# -----------------------------------------------------------------------------
# raylib — fetched and built from source so the same build works on
# Linux/macOS/Windows (MSVC or MinGW) without any vendored binaries.
# -----------------------------------------------------------------------------
include(FetchContent)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
# raylib 5.0 declares cmake_minimum_required(VERSION 3.0); CMake 4.x dropped
# pre-3.5 compatibility. Pin the policy floor so the fetched build still works.
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.5
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(raylib)

# -----------------------------------------------------------------------------
# Sources
# -----------------------------------------------------------------------------
file(GLOB_RECURSE PAMPUCH_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

# WIN32: hide the console window and embed the application icon
if(WIN32)
    enable_language(RC)
    add_executable(Pampuch WIN32 ${PAMPUCH_SOURCES} "${CMAKE_SOURCE_DIR}/resource.rc")
    # WIN32 sets /SUBSYSTEM:WINDOWS (expects WinMain); override entry point to keep main()
    if(MSVC)
        target_link_options(Pampuch PRIVATE /ENTRY:mainCRTStartup)
    endif()
else()
    add_executable(Pampuch ${PAMPUCH_SOURCES})
endif()

target_include_directories(Pampuch PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/deps"
)

target_link_libraries(Pampuch PRIVATE raylib)

# -----------------------------------------------------------------------------
# Compiler flags
# -----------------------------------------------------------------------------
if(MSVC)
    target_compile_options(Pampuch PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(Pampuch PRIVATE -Wall -Wextra -Wno-missing-field-initializers)
endif()

# -----------------------------------------------------------------------------
# Post-build: copy runtime assets next to the executable
# -----------------------------------------------------------------------------
set(RUNTIME_ASSETS
    shaders
    resources
    maps
)

foreach(asset_dir IN LISTS RUNTIME_ASSETS)
    add_custom_command(TARGET Pampuch POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/${asset_dir}"
            "$<TARGET_FILE_DIR:Pampuch>/${asset_dir}"
        COMMENT "Copying ${asset_dir}/ next to executable"
    )
endforeach()
