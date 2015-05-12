/**
 * @addtogroup MyNaiveUtilitiesModule
 * @{
 * @file nt-bitmap.c
 * @brief Naive bitmap's implementation file
 * @details Contains implementation of functions dealing with naive bitmap.
 * @author Tomasz Ostaszewski (ato013)
 * @date 2015-Mar-27
 * @par History
 * <pre>
 * </pre>
 * @sa MyNaiveUtilitiesModule
 * @}
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "nt-bitmap.h"
/**
 * @addtogroup MyNaiveUtilitiesModule
 * @{
 */

/**
 * @brief A data structure that represents bitmap.
 */
struct nt_bitmap {
    unsigned short size_;     /**< Number of bits in the bitmap */
    unsigned long bitmap_[0]; /**< Array of bits from the bitmap */
};

nt_bitmap_t nt_bitmap_create(unsigned int n_size) {
    struct nt_bitmap *ret_val = NULL;
    if (n_size > 0) {
        unsigned long arr_size = 1 + (n_size - 1) / (8 * sizeof(unsigned long));
        ret_val =
            (struct nt_bitmap *)malloc(sizeof(struct nt_bitmap) + sizeof(unsigned long) * arr_size);
        if (NULL != ret_val) {
            memset(&ret_val->bitmap_[0], 0, sizeof(unsigned long) * arr_size);
            ret_val->size_ = arr_size;
        } else {
            errno = ENOMEM;
        }
    } else {
        errno = EINVAL;
    }
    return ret_val;
}

void nt_bitmap_free(nt_bitmap_t a_bitmap) { free(a_bitmap); }

/**
 * @fn FFC
 * @brief returns an index of the first cleared bit of a number.
 */
#if defined MSVC
static inline int FFC(unsigned long x) {
    unsigned long idx;
    _BitScanForward(&idx, ~x);
    return (int)idx;
}
#else
static inline int FFC(unsigned long x) { return __builtin_ffs(~x); }
#endif

unsigned long nt_bitmap_ffc(nt_bitmap_t a_bitmap) {
    unsigned short idx = 0;
    for (; idx < a_bitmap->size_; ++idx) {
        int fc_idx = FFC(a_bitmap->bitmap_[idx]);
        if (0 != fc_idx) {
            return fc_idx + idx * 8 * sizeof(unsigned long) - 1;
        }
    }
    return (unsigned long)-1;
}

unsigned long nt_bitmap_clear(nt_bitmap_t a_bitmap, unsigned short idx) {
    unsigned long arr_idx = idx / (sizeof(unsigned long) * 8);
    if (arr_idx < a_bitmap->size_) {
        unsigned long bit_idx = idx % (sizeof(unsigned long) * 8);
        a_bitmap->bitmap_[arr_idx] &= ~(((unsigned long)1) << bit_idx);
        return 1;
    }
    return 0;
}

unsigned long nt_bitmap_set(nt_bitmap_t a_bitmap, unsigned short idx) {
    unsigned long arr_idx = idx / (sizeof(unsigned long) * 8);
    if (arr_idx < a_bitmap->size_) {
        unsigned long bit_idx = idx % (sizeof(unsigned long) * 8);
        a_bitmap->bitmap_[arr_idx] |= ((unsigned long)1) << bit_idx;
        return 1;
    }
    return 0;
}

unsigned short nt_bitmap_size(nt_bitmap_t_c a_bitmap) { return a_bitmap->size_; }

void nt_bitmap_dump(nt_bitmap_t_c a_bitmap, FILE *out) {
    unsigned short idx;
    fprintf(out, "%6hu ", a_bitmap->size_);
    for (idx = 0; idx < a_bitmap->size_; ++idx) {
        fprintf(out, "%lx ", a_bitmap->bitmap_[idx]);
    }
    fputs("\n", out);
}

/** @} */
