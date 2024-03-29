cmake_minimum_required(VERSION 3.12)


# Pull in PICO SDK (must be before project)
include(pico_sdk_import.cmake)

# Initialize the SDK
pico_sdk_init()

set(PROJECT_NAME pico_neopixel_player)

project(${PROJECT_NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

add_executable(${PROJECT_NAME}
  main.cpp ../raspi_neopixel_player/plasma.cpp ../raspi_neopixel_player/cometrain.cpp ../raspi_neopixel_player/fire.cpp ../raspi_neopixel_player/game_of_life.cpp ../raspi_neopixel_player/pixeltest.cpp
)

#!!
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)

# Make the stdio functions use USB rather than the serial pins
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_pio hardware_adc)

# create map/bin/hex file etc.
pico_add_extra_outputs(${PROJECT_NAME})

#add_dependencies(${PROJECT_NAME} pio_ws2812_datasheet)
