#ifndef MESSAGE_PARSER_H
#define MESSAGE_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ringbuffer.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// 帧头和帧尾的宏定义
#define FRAME_HEADER_1 0x5a
#define FRAME_HEADER_2 0xa5
#define FRAME_TAIL_1   0x5b
#define FRAME_TAIL_2   0xb5

// CRC校验函数声明
uint32_t calculate_crc(const char *data, size_t length);

// 报文解析函数声明
int parse_message(ring_buffer_t *buffer, int ifcrc, char **out_data, size_t *out_data_len);

// 查找帧头
int find_frame_header(ring_buffer_t *buffer);

// 读取消息长度
size_t read_message_length(ring_buffer_t *buffer);

// 查找帧尾
int find_frame_footer(ring_buffer_t *buffer, size_t message_length);

#ifdef __cplusplus
}
#endif

#endif // MESSAGE_PARSER_H
