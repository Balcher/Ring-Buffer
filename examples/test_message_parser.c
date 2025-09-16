/**
 * @file test_message_parser.c
 * @author your name (you@domain.com)
 * @brief C语言版本的测试文件
 * @version 0.1
 * @date 2025-09-16
 * 
 * 
 */
#include <stdio.h>
#include "../message_parser.h"
#include <stdlib.h>

#define BUFFER_SIZE 256

int main() {
    // 初始化环形缓冲区
    char buffer_arr[BUFFER_SIZE];  // 使用 char 类型数组
    ring_buffer_t ring_buffer;
    ring_buffer_init(&ring_buffer, buffer_arr, BUFFER_SIZE);

    // 模拟接收数据
    unsigned char sample_data[] = {0x5a, 0xa5, 0x00, 0x05, 'H', 'e', 'l', 'l', 'o', 0x5b, 0xb5, 0x00, 0x00};
    ring_buffer_queue_arr(&ring_buffer, (char *)sample_data, sizeof(sample_data));

    // 用于接收数据帧
    char *received_data = NULL;
    size_t data_len = 0;

    // 尝试解析报文
    if (parse_message(&ring_buffer, false, &received_data, &data_len)) {
        printf("Message parsed successfully.\n");

        // 打印报文数据
        printf("Received message data: ");
        for (size_t i = 0; i < data_len; i++) {
            // 使用unsigned char打印确保正确输出字节值
            printf("0x%02X ", (unsigned char)received_data[i]);
        }
        printf("\n");

        // 释放分配的内存
        free(received_data);
    } else {
        printf("Failed to parse message.\n");
    }

    return 0;
}
