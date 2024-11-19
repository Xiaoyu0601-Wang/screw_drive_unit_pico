# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_ePaper_SRCS 变量
aux_source_directory(. DIR_ICM42688_SRCS)

include_directories(../Config)

# 生成链接库
add_library(ICM42688 ${DIR_ICM42688_SRCS})
target_link_libraries(ICM42688 PUBLIC Config)