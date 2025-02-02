# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_ePaper_SRCS 变量
aux_source_directory(. DIR_imu_SRCS)

include_directories(../config)
include_directories(../common)

# 生成链接库
add_library(fusion_ahrs ${DIR_imu_SRCS})
target_link_libraries(fusion_ahrs PUBLIC config)