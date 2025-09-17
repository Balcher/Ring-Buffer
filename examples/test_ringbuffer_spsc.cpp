#include <iostream>
#include "../ringbuffer_spsc.h"

int main() {
    const size_t BUF_SIZE = 8; // 必须是2的幂
    char buffer[BUF_SIZE];
    ring_buffer_t rb;

    // 初始化环形缓冲区
    ring_buffer_init(&rb, buffer, BUF_SIZE);

    // 写入单个字符
    std::cout << "Writing single bytes: ";
    for (char c = 'A'; c <= 'H'; ++c) {
        if (ring_buffer_queue(&rb, c)) {
            std::cout << c << " ";
        } else {
            std::cout << "(full) ";
        }
    }
    std::cout << "\n";

    // 写入数组
    const char test_arr[] = "IJKL";
    ring_buffer_size_t written = ring_buffer_queue_arr(&rb, test_arr, sizeof(test_arr)-1);
    std::cout << "Attempted to write IJKL, actually written: " << written << "\n";

    // 读取所有元素
    std::cout << "Reading values: ";
    char val;
    while (ring_buffer_dequeue(&rb, &val)) {
        std::cout << val << " ";
    }
    std::cout << "\n";

    // 再写入更多元素
    const char more[] = "MNOP";
    ring_buffer_queue_arr(&rb, more, sizeof(more)-1);

    // peek 测试
    std::cout << "Peeking values: ";
    for (ring_buffer_size_t i = 0; i < ring_buffer_num_items(&rb); ++i) {
        if (ring_buffer_peek(&rb, &val, i)) {
            std::cout << val << " ";
        }
    }
    std::cout << "\n";

    return 0;
}
