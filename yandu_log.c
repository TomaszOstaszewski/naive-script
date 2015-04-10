/**
 * @file   yandu_log.c
 * @author tomek <tomek@debian.tofuufot.org>
 * @date   Fri Apr 10 23:50:42 2015
 * 
 * @brief  
 * 
 * 
 */

#include "yandu_log.h"

#include <stddef.h>

#if defined DEBUG

int append_formatted_string_to_stream(const char *file, unsigned long line_no, FILE *fstream,
                                      const char *fmt, ...) {
    if (NULL != file) {
        va_list args;
        va_start(args, fmt);
        time_t curr_time = time(NULL);
        char time_buf[32] = {0};
        ctime_r(&curr_time, &time_buf[0]);
        char *newline = strrchr(time_buf, '\n');
        if (newline) {
            *newline = ' ';
        }
        if (fprintf(fstream, "%-30s %5.5lu %s : ", time_buf, line_no, file) > 0 && vfprintf(fstream, fmt, args) >= 0 &&
            fputc('\n', fstream) && 0 == fflush(fstream) && 0 == fsync(fileno(fstream))) {
            return 1;
        }
    }
    return 0;
}

FILE *open_debug_file(const char *dbg_file_name_template) {
    FILE *fs_debug = NULL;
    char *dbg_tmp_file_name = strdup(dbg_file_name_template);
    int debug_fd = mkstemp(dbg_tmp_file_name);
    if (debug_fd < 0) {
        perror("mkstemp");
    } else {
        fs_debug = fdopen(debug_fd, "w");
    }
    free(dbg_tmp_file_name);
    return fs_debug;
}

#else

int append_formatted_string_to_stream(const char *file, unsigned long line_no, FILE *fstream,
                                                    const char *fmt, ...) {
    (void)(file);
    (void)(line_no);
    (void)(fstream);
    (void)(fmt);
    return 0;
}

FILE *open_debug_file(const char *dbg_file_name_template) {
    (void)(dbg_file_name_template);
    return NULL;
}

#endif
