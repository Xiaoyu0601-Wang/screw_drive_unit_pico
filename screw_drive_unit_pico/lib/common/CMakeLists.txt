# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_ePaper_SRCS 变量
aux_source_directory(. DIR_common_SRCS)

include_directories(../config)
include_directories(../../.)

# 生成链接库
add_library(first_order_filter first_order_filter.c)
target_link_libraries(first_order_filter PUBLIC config)