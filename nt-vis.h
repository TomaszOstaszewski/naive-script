/**
 * @addtogroup NtUtilities
 * @{
 * @file nt-vis.h Header file for binary buffer visualize/un-visualize functions.
 * @details Those functions convert be
 * @author Tomasz Ostaszewski (ato013)
 * @date 2015-Mar-31
 * @par History
 * <pre>
 * </pre>
 * @}
 */
#ifndef NT_VIS_H
#define NT_VIS_H

#include <stdint.h>

/**
 * @addtogroup NtUtilities
 * @{
 */

/**
 * @brief Defines a type of conversion performed from input to output
 */
typedef enum nt_vis_format_type_t {
    NT_VIS_FORMAT_HEX,         /**< Output will be in hex digits */
    NT_VIS_FORMAT_C_SYTAX      /**< Output will be a valid C-syntax escaped buffer */
} nt_vis_format_type_t;

/**
 * @brief
 * @details
 * @param format
 * @param[in] buf input buffer to be stringified.
 * @param[in] len length of the input buffer.
 * @param[in,out] p_output reference to a buffer which will be written with
 * stringified" buffer.
 * @param[in] output_size size of the buffer referenced by @c p_output variable.
 * @return Returns a number of characters placed in the buffer. Please note that
 * it may, and in most cases will be, less than number of bytes written to the buffer.
 */
unsigned int nt_vis(nt_vis_format_type_t format, const char* buf, unsigned int len,
                    char* p_output, unsigned int output_size);

/**
 * @brief
 * @details
 * @param format
 * @param buf
 * @param len
 * @param p_output
 * @param output_size
 * @return
 */
unsigned int nt_unvis(nt_vis_format_type_t format, const char* buf, unsigned int len,
                    char* p_output, unsigned int output_size);

/** @} */

#endif /* NT_VIS_H */
