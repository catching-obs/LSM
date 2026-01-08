cmake_minimum_required(VERSION 3.20)

# Main application executable
add_executable(media_app
    main.cpp
)

# Rust library directory
set(RUST_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../target/release)

# Link with core utilities
target_link_libraries(media_app
    core_utils
    rust_bindings
)

# Link with Rust libraries (static libraries built by Cargo)
target_link_libraries(media_app
    ${RUST_LIB_DIR}/libaudio_dsp.a
    ${RUST_LIB_DIR}/libvideo_encoder.a
)

# Platform-specific system libraries
if(UNIX AND NOT APPLE)
    target_link_libraries(media_app pthread dl m)
elseif(APPLE)
    target_link_libraries(media_app pthread dl m)
elseif(WIN32)
    target_link_libraries(media_app ws2_32 userenv)
endif()
