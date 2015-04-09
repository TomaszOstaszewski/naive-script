/**
 * @file yanz_buffer.h
 * @brief Yet Another Zero Copy Buffer
 * @details Header file for Yet Another Zero Copy Buffer
 * @author Tomasz Ostaszewski (ato013)
 * @date 2015-Apr-02
 * @par History
 * <pre>
 * </pre>
 * @copyright Copyright (C) 2015 Motorola Solutions Inc. All rights reserved.
 * Motorola Solutions Confidential Restricted
 */
#ifndef YANZC_BUFFER_H
#define YANZC_BUFFER_H

#include <stdint.h>
#include <stddef.h>

struct io_buffer_t;

typedef struct yanz_read_slice_t {
    unsigned long offset_read_;
    struct io_buffer_t *buffer_;
} yanz_read_slice_t;

typedef struct io_buffer_t {
    unsigned long buf_size_;

    unsigned long offset_write_;
    unsigned long long total_bytes_read_;
    unsigned long long total_bytes_written_;
    unsigned long long reads_count_;
    unsigned long long writes_count_;

    uint8_t *data_;

} io_buffer_t;

struct io_buffer_t *io_buffer_new(unsigned int size) {
    struct io_buffer_t *retval;
    retval = (struct io_buffer_t *)malloc(sizeof(struct io_buffer_t) + sizeof(uint8_t) * size);
    if (NULL != retval) {
        memset(retval, 0, sizeof(struct io_buffer_t) + sizeof(uint8_t) * size);
        retval->data_ = (uint8_t *)retval + sizeof(struct io_buffer_t);
        retval->buf_size_ = size;
    }
    return retval;
}

struct yanz_read_slice_t io_buffer_get_read_slice(struct io_buffer_t *io_buf, unsigned long initial_offset) {
    struct yanz_read_slice_t retval = {.offset_read_ = initial_offset, .buffer_ = io_buf};
    return retval;
}

int io_buffer_realign(struct io_buffer_t* io_buf, struct yanz_read_slice_t* read_slices,
                                    size_t read_slices_size) {
    size_t idx;
    for (idx = 0; idx < read_slices_size; ++idx) {
        if (io_buf->offset_write_ != read_slices[idx].offset_read_) {
            return 0;
        }
    }
    io_buf->offset_write_ = 0;
    for (idx = 0; idx < read_slices_size; ++idx) {
        read_slices[idx].offset_read_ = 0;
    }
    return 1;
}

int io_buffer_is_space_for_writes(const struct io_buffer_t *p_buf) { return p_buf->buf_size_ > p_buf->offset_write_; }

unsigned long io_buffer_get_size_for_writes(const struct io_buffer_t *p_buf) {
    return p_buf->buf_size_ - p_buf->offset_write_;
}
void io_buffer_move_write_offset(struct io_buffer_t *p_buf, unsigned long by) { p_buf->offset_write_ += by; }
uint8_t *io_buffer_get_buf_for_writes(struct io_buffer_t *p_buf) { return &p_buf->data_[p_buf->offset_write_]; }


void yanz_read_slice_move_read_offset(struct yanz_read_slice_t *old_offset,
                                                          unsigned long by) {
    old_offset->offset_read_ += by;
}

int yanz_read_slice_is_space_for_reads(const struct yanz_read_slice_t *p_read_slice) {
    return p_read_slice->buffer_->offset_write_ > p_read_slice->offset_read_;
}

unsigned long yanz_read_slice_get_size_for_reads(const struct yanz_read_slice_t *p_read_slice) {
    return p_read_slice->buffer_->offset_write_ - p_read_slice->offset_read_;
}

uint8_t *yanz_read_slice_get_buf(const struct yanz_read_slice_t *p_read_slice) {
    return &p_read_slice->buffer_->data_[p_read_slice->offset_read_];
}

#endif /* YANZC_BUFFER_H */
