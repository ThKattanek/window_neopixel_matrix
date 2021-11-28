cmake_minimum_required(VERSION 3.0.0)

# read and parse version file
file(READ version PROJECT_VERSION)
string(STRIP ${PROJECT_VERSION} PROJECT_VERSION)
string(REGEX REPLACE "([0-9]+)\\.[0-9]+\\.[0-9]+" "\\1" VERSION_MAJOR ${PROJECT_VERSION})
string(REGEX REPLACE "[0-9]+\\.([0-9]+)\\.[0-9]+" "\\1" VERSION_MINOR ${PROJECT_VERSION})
string(REGEX REPLACE "[0-9]+\\.[0-9]+\\.([0-9]+)" "\\1" VERSION_MICRO ${PROJECT_VERSION})

set(HEADERDEF "${VERSION_MAJOR}_${VERSION_MINOR}_${VERSION_MICRO}")

project(neopixel_player VERSION ${PROJECT_VERSION})

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

set(TEST_TARGET ${PROJECT_NAME})

set(TEST_SOURCES
    main.cpp plasma.cpp cometrain.cpp pictureshow.cpp fire.cpp
)

configure_file(version.h.in version.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(${TEST_TARGET} ${TEST_SOURCES})
target_link_libraries(${TEST_TARGET} ws2811 m png)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
