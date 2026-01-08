cmake_minimum_required(VERSION 3.20)

# Existing test
add_executable(test_core
    test_core.cpp
)
target_link_libraries(test_core core_utils)
add_test(NAME CoreUtilsTest COMMAND test_core)

# New Camera Integration Test
add_executable(test_camera_integration
    test_camera_integration.cpp
)

# Link against core_video (and transitively OpenCV)
target_link_libraries(test_camera_integration
    core_video
)

add_test(NAME CameraIntegrationTest COMMAND test_camera_integration)

# A/V Integration Test
add_executable(test_av_integration
    test_av_integration.cpp
)

target_link_libraries(test_av_integration
    core_video
    core_audio
)

# Audio Loopback Test
add_executable(test_audio_loopback
    test_audio_loopback.cpp
)
target_link_libraries(test_audio_loopback
    core_audio
)

# Device Switching Test
add_executable(test_device_switching
    test_device_switching.cpp
)
target_link_libraries(test_device_switching
    core_video
    core_audio
)
