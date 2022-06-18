cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(tricorder C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

pico_sdk_init()

add_subdirectory(/home/dashy/pico-arduino-compat/libs/adafruit-as7341 build-pac-adafruit-as7341)
add_subdirectory(/home/dashy/pico-arduino-compat/libs/adafruit-bme680-library build-pac-adafruit-bme680-library)
add_subdirectory(/home/dashy/pico-arduino-compat/libs/adafruit-ltr390-library build-pac-adafruit-ltr390-library)
add_subdirectory(/home/dashy/pico-arduino-compat/libs/adafruit-pm25-aqi-sensor build-pac-adafruit-pm25-aqi-sensor)
#add_subdirectory(/home/dashy/pico-arduino-compat/libs/adafruit-busio build-pac-adafruit-busio)

add_library(Tricorder_Sensor Tricorder_Sensor.cpp)
add_library(Tricorder_AS7341 Tricorder_AS7341.cpp)
add_library(Tricorder_BME688 Tricorder_BME688.cpp)
add_library(Tricorder_LTR390 Tricorder_LTR390.cpp)
add_library(Tricorder_PMSA003I Tricorder_PMSA003I.cpp)
add_executable(sensor_manager sensor_manager.cpp)

target_link_libraries(Tricorder_Sensor
	pico_stdlib
	)
target_link_libraries(Tricorder_AS7341 
	pico_stdlib
	pac-adafruit-as7341
	)
target_link_libraries(Tricorder_BME688
	pico_stdlib
	pac-adafruit-bme680-library
	)
target_link_libraries(Tricorder_LTR390
	pico_stdlib
	pac-adafruit-ltr390-library
	)
target_link_libraries(Tricorder_PMSA003I
	pico_stdlib
	pac-adafruit-pm25-aqi-sensor
	)
target_link_libraries(sensor_manager 
	pico_stdlib
	#pac-adafruit-as7341
	Tricorder_Sensor
	Tricorder_AS7341
	Tricorder_BME688
	Tricorder_LTR390
	Tricorder_PMSA003I
	)

pico_add_extra_outputs(sensor_manager)
