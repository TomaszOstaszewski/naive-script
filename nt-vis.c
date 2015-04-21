/**
 * @addtogroup NtUtilities
 * @{
 * @file nt-vis.c
 * @brief 1
 * @details 2
 * @author Tomasz Ostaszewski (ato013)
 * @date 2015-Mar-31
 * @par History
 * <pre>
 * </pre>
 * @}
 */
#include <ctype.h>
#include "nt-vis.h"

/**
 * @addtogroup NtUtilities
 * @{
 */

/** A index to hex digit conversion table. */
static const char s_transTable[] = "0123456789ABCDEF";

/**
 * @brief Minimal buffer size for hex output
 * @details Defines minimal output size buffer accepted by the
 * @ref nt_vis() function with format set to @ref NT_VIS_FORMAT_HEX.
 * For this kind of output 2 bytes are being output for each byte of input.
 * There's also a terminating @c \0 character.
 * This all adds up to at least 5 bytes of output.
 */
#define MINIMAL_BUFFER_SIZE_HEX (3)

/** @brief Minimal buffer size for C syntax output
 * @details Defines minimal output size buffer accepted by the
 * @ref nt_vis() function.
 * This function output data in chunks, a single chunk ranging from 1
 * (single printable character) to 4 bytes
 * (escape + 3 octal digits for non-printable characters).
 * There's also a terminating @c \0 character.
 * This all adds up to at least 5 bytes of output.
 */
#define MINIMAL_BUFFER_SIZE_C (5)

unsigned int nt_vis(nt_vis_format_type_t format, const char* buf, unsigned int len, char* p_output,
                    unsigned int output_size) {
    /* Number of characters printed */
    /* Note: One character can take up to 4 bytes (byte 0x00 is printed as \000), */
    /* hence characters count is often less than bytes printed */
    unsigned int out_char_count = 0;
    /* Number of bytes printed */
    unsigned int text_len = 0;
    unsigned int i; /* Index of the current byte printed */
    /* Now print out escaped buffer */
    switch (format) {
    case NT_VIS_FORMAT_HEX:
        if (output_size > MINIMAL_BUFFER_SIZE_HEX) {
            for (i = 0; i < len && text_len < output_size - MINIMAL_BUFFER_SIZE_HEX; i++) {
                p_output[text_len++] = s_transTable[(((unsigned char)buf[i]) >> 4) & 0x0f];
                p_output[text_len++] = s_transTable[((unsigned char)buf[i]) & 0x0f];
                out_char_count += 2;
            }
            /* Add terminating NULL */
            p_output[text_len++] = 0;
        }
        break;
    case NT_VIS_FORMAT_C_SYTAX:
    default:
        if (output_size > MINIMAL_BUFFER_SIZE_C) {
            for (i = 0; i < len && text_len < output_size - MINIMAL_BUFFER_SIZE_C; i++) {
                switch (buf[i]) {
                case '"': /* Escape " */
                    p_output[text_len++] = '\\';
                    p_output[text_len++] = '"';
                    out_char_count++;
                    break;
                case '\\': /* Escape \ */
                    p_output[text_len++] = '\\';
                    p_output[text_len++] = '\\';
                    out_char_count++;
                    break;
                default:
                    if (isprint(buf[i])) {
                        p_output[text_len++] = buf[i];
                        out_char_count++;
                    } else {
                        out_char_count++;
                        p_output[text_len++] = '\\';
                        /* If possible, encode using C escape sequences rather than                 */
                        /* octal escape seqcuences - so we output '\a' rather than '\012' for 0x10  */
                        /* byte                                                                     */
                        switch (buf[i]) {
                        case '\a':
                            p_output[text_len++] = 'a';
                            break;
                        case '\b':
                            p_output[text_len++] = 'b';
                            break;
                        case '\f':
                            p_output[text_len++] = 'f';
                            break;
                        case '\n':
                            p_output[text_len++] = 'n';
                            break;
                        case '\r':
                            p_output[text_len++] = 'r';
                            break;
                        case '\t':
                            p_output[text_len++] = 't';
                            break;
                        case '\v':
                            p_output[text_len++] = 'v';
                            break;
                        case '\0':
                            p_output[text_len++] = '0';
                            break;
                            /* gcc has an extension that treats \e as a single escape character. */
#if defined __GNUC__
                        case '\e':
                            p_output[text_len++] = 'e';
                            break;
#endif
                        default:
                            /* Fallback on octal escape sequences for all   */
                            /* other characters that cannot be represented  */
                            /* using C escape sequences                     */
                            p_output[text_len++] =
                                s_transTable[(((unsigned char)buf[i]) >> 6) & 0x03];
                            p_output[text_len++] =
                                s_transTable[(((unsigned char)buf[i]) >> 3) & 0x07];
                            p_output[text_len++] = s_transTable[((unsigned char)buf[i]) & 0x07];
                            break;
                        }
                    }
                    break;
                }
            }
            /* Add terminating NULL */
            p_output[text_len++] = 0;
        }
        break;
    }
    return out_char_count;
}

/** @} */
