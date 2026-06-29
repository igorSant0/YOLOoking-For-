file(GLOB SOURCES *.cpp)

idf_component_register(SRCS "${SOURCES}"
                       INCLUDE_DIRS "."  
                        REQUIRES arduino-esp32 esp_driver_touch_sens mbedtls)    