# 默认使用 gcc 编译 C 程序，但对于 C++ 使用 g++
CC             = g++
CFLAGS         = -Wall -g -O2 -std=c++11  # C++ 编译选项

# 默认目标
all: simple test_message_parser test_message_parser_cpp tail

# 编译 simple 可执行文件
simple: ../ringbuffer.c simple.c
	$(CC) $(CFLAGS) -o simple simple.c ../ringbuffer.c

# 编译 tail 可执行文件
tail: ../ringbuffer.c tail.c
	$(CC) $(CFLAGS) -o tail tail.c ../ringbuffer.c

# 编译 C 版本的 test_message_parser 可执行文件
test_message_parser: ../ringbuffer.c ../message_parser.c test_message_parser.c
	$(CC) $(CFLAGS) -o test_message_parser test_message_parser.c ../ringbuffer.c ../message_parser.c

# 编译 C++ 版本的 test_message_parser 可执行文件
test_message_parser_cpp: ../ringbuffer.c ../message_parser.c test_message_parser.cpp
	$(CC) $(CFLAGS) -o test_message_parser_cpp test_message_parser.cpp ../ringbuffer.c ../message_parser.c

# 清理编译生成的文件
clean:
	rm -f simple tail test_message_parser test_message_parser_cpp
