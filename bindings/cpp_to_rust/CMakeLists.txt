cmake_minimum_required(VERSION 3.20)

# FFI bindings library
add_library(rust_bindings STATIC
    camera_ffi.cpp
    audio_ffi.cpp
)

target_include_directories(rust_bindings PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link against core_video to access CameraSource
target_link_libraries(rust_bindings PUBLIC
    core_video
    core_audio
)
