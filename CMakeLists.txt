# --------------------------------------------------------------------------
# Dependencies
# --------------------------------------------------------------------------

find_package(yat CONFIG REQUIRED)
find_package(pvcam CONFIG REQUIRED)

# --------------------------------------------------------------------------
# Collect sources and includes
# --------------------------------------------------------------------------

file(GLOB_RECURSE sources src/*.cpp)
set(includedirs
    include/
)

# --------------------------------------------------------------------------
# Target
# --------------------------------------------------------------------------

add_library(limaroperscientific ${sources})

target_include_directories(limaroperscientific
    PUBLIC 
        ${includedirs}
)

target_link_libraries(limaroperscientific 
    PUBLIC
        limacore
        yat::yat
        pvcam::pvcam
)

limatools_set_library_soversion(limaroperscientific "VERSION")

# --------------------------------------------------------------------------
# Compile definitions
# --------------------------------------------------------------------------

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" PRINCETON_VERSION_RAW)
string(STRIP "${PRINCETON_VERSION_RAW}" PRINCETON_VERSION)

target_compile_definitions(limaroperscientific
    PUBLIC
        PRINCETON_NAME=Princeton
        PRINCETON_VERSION=${PRINCETON_VERSION}
        PRINCETON_ENABLED
)

message(STATUS "Camera enabled: Princeton ${PRINCETON_VERSION}")

# Windows DLL export
if (WIN32 AND BUILD_SHARED_LIBS)
    target_compile_definitions(limaroperscientific
        PRIVATE
            LIBROPERSCIENTIFIC_EXPORTS
    )
endif()

# --------------------------------------------------------------------------
# Install
# --------------------------------------------------------------------------

install(
    TARGETS limaroperscientific 
    LIBRARY DESTINATION lib
)
