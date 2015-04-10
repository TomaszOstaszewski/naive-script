/**
 * @file yanzc_buffer.h
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

/**
 * @brief Defines a read slice of a buffer.
 * @details A single buffer can be written by only one writer at a time,
 * but it can be read by many readers at the same time. Each reader
 * keeps an instance of this structure so it servers as a bookmark where
 * where the last read has stopped.
 */
typedef struct yanz_read_slice_t {
    /**
     * @brief Points to the next byte being read.
     */
    unsigned long offset_read_;
    /**
     * @brief Pointer to the actual buffer from where
     * data is to be read.
     */
    struct yanzc_buffer_t *buffer_;
} yanz_read_slice_t;

/**
 * @brief Yet another zero copy buffer definition.
 */
typedef struct yanzc_buffer_t {
    /**
     * @brief Total size of the buffer.
     */
    unsigned long buf_size_;
    /**
     * @brief Write offset.
     * @details Points to the place where next data will be
     * written. 
     */
    unsigned long offset_write_;
    /**
     * @brief Pointer to the location where data is stored.
     */
    uint8_t *data_;
} yanzc_buffer_t;

struct yanzc_buffer_t *io_buffer_new(unsigned int size) {
    struct yanzc_buffer_t *retval;
    size_t alloc_size = sizeof(struct yanzc_buffer_t) + sizeof(uint8_t) * size;
    retval = (struct yanzc_buffer_t *)malloc(alloc_size);
    if (NULL != retval) {
        memset(retval, 0, alloc_size);
        retval->data_ = (uint8_t *)retval + sizeof(struct yanzc_buffer_t);
        retval->buf_size_ = size;
    }
    return retval;
}

struct yanz_read_slice_t io_buffer_get_read_slice(struct yanzc_buffer_t *io_buf,
                                                  unsigned long initial_offset) {
    struct yanz_read_slice_t retval = {.offset_read_ = initial_offset, .buffer_ = io_buf};
    return retval;
}

int io_buffer_realign(struct yanzc_buffer_t *io_buf, struct yanz_read_slice_t *read_slices,
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

int io_buffer_is_space_for_writes(const struct yanzc_buffer_t *p_buf) {
    return p_buf->buf_size_ > p_buf->offset_write_;
}

unsigned long io_buffer_get_size_for_writes(const struct yanzc_buffer_t *p_buf) {
    return p_buf->buf_size_ - p_buf->offset_write_;
}
void io_buffer_move_write_offset(struct yanzc_buffer_t *p_buf, unsigned long by) {
    p_buf->offset_write_ += by;
}
uint8_t *io_buffer_get_buf_for_writes(struct yanzc_buffer_t *p_buf) {
    return &p_buf->data_[p_buf->offset_write_];
}

void yanz_read_slice_move_read_offset(struct yanz_read_slice_t *old_offset, unsigned long by) {
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

int from_fd_to_buffer(int fd, struct yanzc_buffer_t *io_buf) {
    if (io_buffer_is_space_for_writes(io_buf)) {
        int result = 0;
        do {
            result = read(fd, io_buffer_get_buf_for_writes(io_buf), io_buffer_get_size_for_writes(io_buf));
        } while (-1 == result && EINTR == evutil_socket_geterror(fd));
        if (result > 0) {
//            LOG_DEBUG(g_fs_debug, "%d %lu %lu", fd, (unsigned long)result, io_buffer_get_size_for_writes(io_buf));
            io_buffer_move_write_offset(io_buf, (unsigned long)result);
        } else if (-1 == result && EAGAIN == evutil_socket_geterror(fd)) {
            return 0;
        } else {
//            LOG_DEBUG(g_fs_debug, "%d %s", errno, strerror(errno));
            return -1;
        }
    }
    return 0;
}

int from_buffer_to_fd(struct yanz_read_slice_t *p_read_slice, int fd) {
    if (yanz_read_slice_is_space_for_reads(p_read_slice)) {
        int result = 0;
        do {
            result = write(fd, yanz_read_slice_get_buf(p_read_slice), yanz_read_slice_get_size_for_reads(p_read_slice));
        } while (-1 == result && EINTR == evutil_socket_geterror(fd));
        if (result > 0) {
//            LOG_DEBUG(g_fs_debug, "%d %lu %lu", fd, (unsigned long)result, yanz_read_slice_get_size_for_reads(p_read_slice));
            yanz_read_slice_move_read_offset(p_read_slice, (unsigned long)result);
        } else if (-1 == result && EAGAIN == evutil_socket_geterror(fd)) {
            return 0;
        } else {
//            LOG_DEBUG(g_fs_debug, "%d %s", errno, strerror(errno));
            return errno;
        }
    }
    return 0;
}

#endif /* YANZC_BUFFER_H */
