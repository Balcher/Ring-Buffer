#include "message_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// CRC32 校验计算
uint32_t calculate_crc(const char *data, size_t length) {
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < length; i++) {
        crc ^= (uint32_t)(data[i]);
        for (int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
    }
    return ~crc;
}

// 查找帧头，使用宏定义的默认值
int find_frame_header(ring_buffer_t *buffer) {
    while (ring_buffer_num_items(buffer) >= 1) {  // 确保至少有1个字节
        unsigned char byte1;
        ring_buffer_dequeue(buffer, (char*)&byte1);

        // 打印读取的字节信息
        printf("Byte1: 0x%02X\n", byte1);

        if (byte1 == FRAME_HEADER_1) {  // 如果第一个字节匹配帧头的第一个字节
            // 读取第二个字节并检查
            if (ring_buffer_num_items(buffer) >= 1) {
                unsigned char byte2;
                ring_buffer_dequeue(buffer, (char*)&byte2);
                printf("Byte2: 0x%02X\n", byte2);

                if (byte2 == FRAME_HEADER_2) {
                    return 1;  // 找到完整的帧头
                } else {
                    // 如果第二个字节不匹配，回退一个字节
                    buffer->tail_index = (buffer->tail_index - 1) & buffer->buffer_mask;
                }
            }
        }
        // 继续前进，检查下一个字节
        buffer->tail_index = (buffer->tail_index + 1) & buffer->buffer_mask;
    }

    return 0; // 未找到帧头
}


// 读取消息长度（假设长度字段为2字节）
size_t read_message_length(ring_buffer_t *buffer) {
    unsigned char byte1, byte2;
    ring_buffer_dequeue(buffer, (char*)&byte1);
    ring_buffer_dequeue(buffer, (char*)&byte2);

    size_t length = (size_t)((uint8_t)byte1 << 8 | (uint8_t)byte2); // 返回2字节表示的长度
    printf("Message Length: %zu\n", length); // 打印消息长度
    return length;
}

// 查找帧尾，使用宏定义的默认值
int find_frame_footer(ring_buffer_t *buffer, size_t message_length) {
    unsigned char footer1, footer2;
    ring_buffer_dequeue(buffer, (char*)&footer1);
    ring_buffer_dequeue(buffer, (char*)&footer2);

    // 打印帧尾信息
    printf("Footer1: 0x%02X, Footer2: 0x%02X\n", footer1, footer2);

    if (footer1 == FRAME_TAIL_1 && footer2 == FRAME_TAIL_2) { // 使用宏定义的帧尾
        return 1; // 找到帧尾
    }

    return 0; // 未找到帧尾
}

// 解析报文：查找帧头、读取长度字段、查找帧尾并验证CRC
int parse_message(ring_buffer_t *buffer, int ifcrc, char **received_data, size_t *data_len) {
    // 查找帧头
    if (!find_frame_header(buffer)) {
        printf("Failed to find frame header\n");
        return 0; // 未找到帧头
    }

    // 读取消息长度
    size_t message_length = read_message_length(buffer);

    // 打印缓冲区中的字节数，确保有足够的字节
    printf("Buffer items available: %zu, Expected message length: %zu\n", ring_buffer_num_items(buffer), message_length);

    // 读取数据
    *received_data = (char*)malloc(message_length);
    if (*received_data == NULL) {
        printf("Memory allocation failed\n");
        return 0;
    }

    size_t bytes_read = ring_buffer_dequeue_arr(buffer, *received_data, message_length);
    if (bytes_read != message_length) {
        printf("Warning: Not enough bytes read from ring buffer, expected %zu, got %zu\n", message_length, bytes_read);
    }

    // 打印读取的数据并解释每个字节的意思
    printf("Received Data: ");
    for (size_t i = 0; i < bytes_read; i++) {
        unsigned char byte = (unsigned char)(*received_data)[i];
        
        // 打印字节的十六进制表示
        printf("0x%02X ", byte);
        
        // 打印字节的字符表示（如果是可打印字符）
        if (byte >= 32 && byte <= 126) { // 判断是否是可打印字符范围
            printf("('%c') ", byte);
        } else {
            printf("(non-printable) ");
        }
    }
    printf("\n");

    // 查找帧尾
    if (!find_frame_footer(buffer, message_length)) {
        printf("Failed to find frame footer\n");
        return 0; // 未找到帧尾
    }

    // 打印读取的数据
    printf("Data: ");
    for (size_t i = 0; i < bytes_read; i++) {
        printf("0x%02X ", (unsigned char)(*received_data)[i]);
    }
    printf("\n");

    *data_len = bytes_read;

    // 如果需要进行 CRC 校验
    if (ifcrc) {
        uint32_t crc = calculate_crc(*received_data, message_length);

        // 假设 CRC 字段紧跟在数据后面
        uint32_t received_crc;
        ring_buffer_dequeue_arr(buffer, (char*)&received_crc, sizeof(received_crc));

        printf("Calculated CRC: 0x%08X, Received CRC: 0x%08X\n", crc, received_crc);

        if (crc == received_crc) {
            printf("Received valid message\n");
            return 1; // 成功解析并验证
        } else {
            printf("CRC error!\n");
            free(*received_data);
            return 0; // CRC 验证失败
        }
    } else {
        // 不执行 CRC 校验，直接返回成功
        printf("Received valid message (no CRC check)\n");
        return 1; // 成功解析，没有 CRC 校验
    }
}
