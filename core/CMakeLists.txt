cmake_minimum_required(VERSION 3.20)

# Core utils library
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/utils/logger.cpp")
    add_library(core_utils STATIC
        utils/logger.cpp
        utils/memory_pool.cpp
    )
    target_include_directories(core_utils PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

# Video Core Library
find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

add_library(core_audio STATIC
    audio/MicrophoneSource.cpp
    audio/SpeakerSink.cpp
)
target_include_directories(core_audio PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/audio
    ${SDL2_INCLUDE_DIRS}
)
target_link_directories(core_audio PUBLIC ${SDL2_LIBRARY_DIRS})
target_link_libraries(core_audio PUBLIC
    ${SDL2_LIBRARIES}
)

add_library(core_video STATIC
    video/CameraSource.cpp
    video/ScreenSource.cpp
    video/SourceManager.cpp
)

target_include_directories(core_video PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/video
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(core_video PUBLIC ${OpenCV_LIBS})

# Streaming Library
add_library(core_streaming STATIC
    streaming/StreamController.cpp
)

target_include_directories(core_streaming PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/streaming
)

# Test Executable (Module Test)
add_executable(video_module_test main_video_test.cpp)
target_link_libraries(video_module_test 
    PRIVATE 
    core_video 
    core_streaming
)
if(UNIX)
    target_link_libraries(video_module_test PRIVATE pthread)
endif()
