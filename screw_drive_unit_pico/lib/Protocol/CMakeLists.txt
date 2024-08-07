# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_ePaper_SRCS 变量
aux_source_directory(. DIR_Protocol_SRCS)

include_directories(../Config)
include_directories(../MCP2515)

# 生成链接库
add_library(Protocol ${DIR_Protocol_SRCS})
target_link_libraries(Protocol PUBLIC pico_stdlib Config MCP2515)