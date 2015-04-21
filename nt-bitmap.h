/**
 * @addtogroup MyNaiveUtilitiesModule
 * @{
 * @file nt-bitmap.h
 * @brief Naive bitmap's header file
 * @details Contains macros and declarations for functions dealing with naive bitmap.
 * @author Tomasz Ostaszewski (ato013)
 * @date 2015-Mar-27
 * @par History
 * <pre>
 * </pre>
 * @sa MyNaiveUtilitiesModule
 * @}
 */

#ifndef NT_BITMAP_H
#define NT_BITMAP_H

/**
 * @defgroup MyNaiveUtilitiesModule My Naive utilities
 * @brief Some naive, yet ubiquituous utilities.
 */


/**
 * @addtogroup MyNaiveUtilitiesModule
 * @{
 */

/**
 * @brief A handle of a bitmap.
 * @details This definition hides the actual layout of the bitmap from its clients.
 */
typedef struct nt_bitmap* nt_bitmap_t;
typedef const struct nt_bitmap* nt_bitmap_t_c;

/**
 * @brief Creates a naive bitmap.
 * @details This is a C language constructor.
 * @attention The @c n_size parameter must be greater than zero, otherwise
 * the creation fails.
 * @param n_size number of bits of the underlying bitmap.
 * @return Returned values:
 * - a non @c NULL value; @n Indicates a success;
 * - a @c NULL value; @n Indicates a failure, usually the @c n_size parameter is out of limits. @n
 * In that case, the variable @c errno is set to a non-zero value.
 * @sa nt_bitmap_free()
 */
nt_bitmap_t nt_bitmap_create(unsigned int n_size);

/**
 * @brief Destroys a naive bitmap.
 * @details This is C language desctructor. It reclaims all the resources
 * claimed during a call to @ref nt_bitmap_create().
 * @param a_bitmap bitmap which is to be destroyed.
 * @sa nt_bitmap_create()
 */
void nt_bitmap_free(nt_bitmap_t a_bitmap);

/**
 * @brief Find first cleared bit.
 * @details Returns a position of a first cleared bit in the bitmap.
 * @param a_bitmap
 * @return Returned values:
 * - when all bits in the bitmap are set, it returns <tt>(unsigned long)-1</tt>
 * - otherwise, it returns index of the first clear bit.
 */
unsigned long nt_bitmap_ffc(nt_bitmap_t a_bitmap);

/**
 * @brief Clears a bit in a bitmap.
 * @details Clears a selected bit in a bitmap.
 * @param a_bitmap
 * @param idx
 * @return If the @c idx parameter is valid, i.e. in range from 0 to
 * bitmap size minus 1, both inclusive, then the bit is cleared and returned value is 1.
 * Otherwise, if the @c idx parameter exceeds underlying bitmap size, then bitmap is
 * unaltered and returned value is 0.
 * @sa nt_bitmap_size()
 */
unsigned long nt_bitmap_clear(nt_bitmap_t a_bitmap, unsigned short idx);

/**
 * @brief Sets a selected bit in a bitmap.
 * @param a_bitmap a bitmap whose bit is to be set.
 * @param idx index of a bit to be cleared.
 * @return If the @c idx parameter is valid, i.e. in range from 0 to
 * bitmap size minus 1, both inclusive, then the bit is set and returned value is 1.
 * Otherwise, if the @c idx parameter exceeds underlying bitmap size, then bitmap is
 * unaltered and returned value is 0.
 * @sa nt_bitmap_size()
 */
unsigned long nt_bitmap_set(nt_bitmap_t a_bitmap, unsigned short idx);

/**
 * @brief Returns size, i.e. number of total bits that can be manipulated,
 * of a given bitmap.
 * @param a_bitmap a bitmap whose size is to be returned
 * @return Returns number of bits held in the underlying bitmap.
 */
unsigned short nt_bitmap_size(nt_bitmap_t_c a_bitmap);

#if defined DEBUG
#include <stdio.h>
/**
 * @brief
 * @details
 * @param a_bitmap
 * @param out
 */
void nt_bitmap_dump(nt_bitmap_t_c a_bitmap, FILE* out);

#define NT_BITMAP_DUMP(bmp, sink) do { nt_bitmap_dump(bmp, sink); } while(0)

#else

#define NT_BITMAP_DUMP(bmp, sink) do { } while(0)

#endif


/** @} */

#endif /* NT_BITMAP_H */
