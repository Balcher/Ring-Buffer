#include "ringbuffer_spsc.h"

void ring_buffer_init(ring_buffer_t *rb, char *buf, size_t buf_size) {
    RING_BUFFER_ASSERT(RING_BUFFER_IS_POWER_OF_TWO(buf_size));
    rb->buffer = buf;
    rb->buffer_mask = buf_size - 1;
    rb->head_index.store(0, std::memory_order_relaxed);
    rb->tail_index.store(0, std::memory_order_relaxed);
}

bool ring_buffer_queue(ring_buffer_t *rb, char data) {
    auto head = rb->head_index.load(std::memory_order_relaxed);
    auto tail = rb->tail_index.load(std::memory_order_acquire);

    if (((head - tail) & rb->buffer_mask) == rb->buffer_mask) {
        return false; // full
    }

    rb->buffer[head & rb->buffer_mask] = data;
    rb->head_index.store(head + 1, std::memory_order_release);
    return true;
}

ring_buffer_size_t ring_buffer_queue_arr(ring_buffer_t *rb, const char *data, ring_buffer_size_t size) {
    ring_buffer_size_t i = 0;
    for (; i < size; i++) {
        if (!ring_buffer_queue(rb, data[i]))
            break;
    }
    return i;
}

bool ring_buffer_dequeue(ring_buffer_t *rb, char *data) {
    auto tail = rb->tail_index.load(std::memory_order_relaxed);
    auto head = rb->head_index.load(std::memory_order_acquire);

    if (tail == head) {
        return false; // empty
    }

    *data = rb->buffer[tail & rb->buffer_mask];
    rb->tail_index.store(tail + 1, std::memory_order_release);
    return true;
}

ring_buffer_size_t ring_buffer_dequeue_arr(ring_buffer_t *rb, char *data, ring_buffer_size_t len) {
    ring_buffer_size_t i = 0;
    for (; i < len; i++) {
        if (!ring_buffer_dequeue(rb, &data[i]))
            break;
    }
    return i;
}

bool ring_buffer_peek(ring_buffer_t *rb, char *data, ring_buffer_size_t index) {
    auto tail = rb->tail_index.load(std::memory_order_acquire);
    auto head = rb->head_index.load(std::memory_order_acquire);

    if (index >= ((head - tail) & rb->buffer_mask)) {
        return false; // out of range
    }

    *data = rb->buffer[(tail + index) & rb->buffer_mask];
    return true;
}

extern inline bool ring_buffer_is_empty(ring_buffer_t *rb);
extern inline bool ring_buffer_is_full(ring_buffer_t *rb);
extern inline ring_buffer_size_t ring_buffer_num_items(ring_buffer_t *rb);