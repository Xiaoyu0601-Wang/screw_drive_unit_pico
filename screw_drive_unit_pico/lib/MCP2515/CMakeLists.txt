# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_ePaper_SRCS 变量
aux_source_directory(. DIR_MCP2515_SRCS)

include_directories(../Config)

# 生成链接库
add_library(MCP2515 ${DIR_MCP2515_SRCS})
target_link_libraries(MCP2515 PUBLIC Config)