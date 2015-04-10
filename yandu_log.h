/**
 * @file   yandu_log.h
 * @author tomek <tomek@debian.tofuufot.org>
 * @date   Fri Apr 10 23:48:09 2015
 * 
 * @brief  YANDU LOG - Yet another debug utility with logging header file.
 * 
 * 
 */

#ifndef YANDU_LOG_H
#define YANDU_LOG_H

#include <stdio.h>

/** 
 * @brief
 * @details
 * @param file 
 * @param line_no 
 * @param fstream 
 * @param fmt 
 * 
 * @return 
 */
int append_formatted_string_to_stream(const char *file, unsigned long line_no, FILE *fstream,
                                                    const char *fmt, ...) __attribute__((format(printf, 4, 5)));

/**
 * @brief Creates a stream for logging debug output.
 * @details
 * @param[in] dbg_file_name_template
 * @return
 */
FILE *open_debug_file(const char *dbg_file_name_template);

/**
 * @def LOG_DEBUG
 * @brief A helper macro that that sprinkles logs code execution artifacts
 * to an output stream.
 * @details Macro expansion depends on the type of build you have.
 * - In @c DEBUG builds it expands to a valid calls that appends a formatted string
 * to an @c fstream parameter.
 * - In @c NDEBUG builds it expands to nothing, no operation.
 */
#if !defined NDEBUG

#define LOG_DEBUG(fstream, ...)                                                                                        \
    do {                                                                                                               \
        append_formatted_string_to_stream(__func__, __LINE__, fstream, ##__VA_ARGS__);                                 \
    } while (0)

#else

#define LOG_DEBUG(fstream, ...)                                                                                        \
    do {                                                                                                               \
    } while (0)

#endif

#endif /* YANDU_LOG_H */
