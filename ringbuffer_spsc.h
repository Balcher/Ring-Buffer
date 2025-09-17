/*
 * @Description: 通用的跨平台无锁SPSC环形缓冲区
 * @Author: BiChunkai 321521004@qq.com
 * @Date: 2025-09-17 08:54:22
 * @LastEditTime: 2025-09-17 09:51:19
 * @FilePath: /utils/3rdParty/Ring-Buffer/ringbuffer_spsc.h
 *
 * Copyright (c) 2025 by BiChunkai 321521004@qq.com, All Rights Reserved.
 */
#ifndef RINGBUFFER_SPSC_H
#define RINGBUFFER_SPSC_H

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cassert>

#ifdef __cplusplus
extern "C"
{
#endif

#define RING_BUFFER_ASSERT(x) assert(x)

/**
 * Checks if the buffer_size is a power of two.
 * Due to the design only <tt> RING_BUFFER_SIZE-1 </tt> items
 * can be contained in the buffer.
 * buffer_size must be a power of two.
 */
#define RING_BUFFER_IS_POWER_OF_TWO(buffer_size) ((buffer_size & (buffer_size - 1)) == 0)

    /** Type for index and size */
    typedef size_t ring_buffer_size_t;

    /**Forward declare */
    typedef struct ring_buffer_t ring_buffer_t;

    /**
     * Ring buffer structure (SPSC, lock-free, atmoic operations)
     */
    struct ring_buffer_t
    {
        char *buffer;                               /**< Buffer memory */
        ring_buffer_size_t buffer_mask;             /**buffer_size - 1 (for bit masking) */
        std::atomic<ring_buffer_size_t> tail_index; /**< Index of tail (dequeue) */
        std::atomic<ring_buffer_size_t> head_index; /**< Index of head (enqueue) */
    };

    /* API */
    void ring_buffer_init(ring_buffer_t *rb, char *buf, size_t buf_size);
    bool ring_buffer_queue(ring_buffer_t *rb, char data);
    ring_buffer_size_t ring_buffer_queue_arr(ring_buffer_t *rb, const char *data, ring_buffer_size_t size);
    bool ring_buffer_dequeue(ring_buffer_t *rb, char *data);
    ring_buffer_size_t ring_buffer_dequeue_arr(ring_buffer_t *rb, char *data, ring_buffer_size_t len);
    bool ring_buffer_peek(ring_buffer_t *rb, char *data, ring_buffer_size_t index);

    inline bool ring_buffer_is_empty(ring_buffer_t *rb)
    {
        return (rb->head_index.load(std::memory_order_acquire) ==
                rb->tail_index.load(std::memory_order_acquire));
    }

    inline bool ring_buffer_is_full(ring_buffer_t *rb)
    {
        auto head = rb->head_index.load(std::memory_order_acquire);
        auto tail = rb->tail_index.load(std::memory_order_acquire);
        return ((head - tail) & rb->buffer_mask) == rb->buffer_mask;
    }

    inline ring_buffer_size_t ring_buffer_num_items(ring_buffer_t *rb)
    {
        auto head = rb->head_index.load(std::memory_order_acquire);
        auto tail = rb->tail_index.load(std::memory_order_acquire);
        return ((head - tail) & rb->buffer_mask);
    }

#ifdef __cplusplus
}
#endif

#endif