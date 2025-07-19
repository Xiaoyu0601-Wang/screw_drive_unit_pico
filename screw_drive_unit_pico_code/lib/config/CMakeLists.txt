# 并将名称保存到 DIR_Config_SRCS 变量
aux_source_directory(. DIR_config_SRCS)

# set(PICO_CYW43_SUPPORTED TRUE)

# 生成链接库
add_library(config ${DIR_config_SRCS})
target_link_libraries(config 
                      PUBLIC
                      pico_stdlib
                      hardware_clocks
                      hardware_spi
                      hardware_i2c
                      hardware_pwm
                      hardware_adc
                      pico_unique_id
                      hardware_flash)
if (PICO_CYW43_SUPPORTED)
    target_link_libraries(config 
                          pico_cyw43_arch_none)
endif()
