/**
 * @file pseudoshell.c
 * @brief A toy 'script' like utility.
 * @details This toy program is an example how pseudo terminals can
 * be used. When started, it starts a new shell. Whatever is typed in this new shell
 * is saved to a temporary file.
 * @author Tomasz Ostaszewski (ato013)
 * @date 2015-Apr-01
 * @par History
 * <pre>
 * </pre>
 * @copyright Copyright (C) 2015 Motorola Solutions Inc. All rights reserved.
 * Motorola Solutions Confidential Restricted
 */

/**
 * @mainpage Script like utility
 * @tableofcontents
 * @section TheIntroduction Introduction - what it is all about.
 * The idea here is to create a simple application that saves all the conversation between a
 * computer
 * user and its terminal into a file. This sounds simple enough, but in reality can be a bit
 * challenging. @n
 * @section TheNaiveApproach A naive approach
 * A naive approach would be as follows:
 * -# Start this application; @n
 * Well, that's simple enough.
 * -# Create a bidirectional communication channel; @n
 * This can be a pair of sockets or a set of named pipes. The idea here is that this bidirectional
 * channel
 * allows 2 processes to communicate with each other.
 * -# <tt><a href="http://linux.die.net/man/3/fork">fork()</a></tt> current process; @n
 *    - In the parent process, run an function that forward everything form the standard input into
 * the communication channel, and then everything it has received from the communication channel
 * back to the
 * standard output.
 *    - In the child process, close your standard input and standard output and replace them with
 * the endpoints
 * from your communication channel. Then call your shell via <tt><a
 * href="http://linux.die.net/man/2/execve">execve()</a></tt>.
 *
 * @subsection TheNaiveApproachProblems Problems with the naive approach
 * This kind of application would run, but it can hardly be called a reliable work. Here's a short
 * list of problems with
 * this kind of @a solution:
 * - launched shell does not have a prompt sign; @n
 * - some utilities like <tt><a href="http://linux.die.net/man/1/tty">tty(1)</a></tt> do not work
 * correctly.
 * - editors do not work correctly; @n
 * Trying to launch editor like @c vi causes it to report that standard output
 * with all the resulting annoyances.
 *
 * All the problems above can be attributed to the fact that a standard input and a standard output
 * of a child shell process are not terminal devices. Those are either
 * <a href="http://linux.die.net/man/2/socket">sockets<a>
 * or <a href="http://linux.die.net/man/7/pipe">pipes</a>
 * or <a href="http://linux.die.net/man/3/mkfifo">FIFO queues</a>.
 * As a result, the special library function
 * <tt><a href="http://linux.die.net/man/3/isatty">isatty()</a></tt>
 * returns 0 for those kind of descriptors, precluding any meaningful terminal work (no backspace
 * available, no addresable cursor, no random screen position access and so on). @n
 * In order for programs which require such features (editors like
 * <tt><a href="http://www.vim.org/">VIM</a></tt> or
 * <tt><a href="https://www.gnu.org/software/emacs/">GNU Emacs</a></tt>) to work correctly,
 * it is essential for their standard input and output to be associated with a
 * a terminal device. @n
 * Although a terminal device is represented by a file descriptor just like regular files are,
 * the set of operations available for terminals is quite different
 * than for regular files.
 * For instance, by default terminals operate in the cooked mode, which means
 * that they do not provide input until someone enters a newline character. In addition the
 * <tt><a href="http://man7.org/linux/man-pages/man2/ioctl.2.html">ioctl's()</a></tt> for terminals
 * are quite different from those available to regular files.
 * @section TheImprovedApproach The improved approach
 * This time we are a bit smarter. We use
 * <a href="http://linux.die.net/man/7/pty">the pty subsystem</a>.
 * This allows us to create a @a pseudoterminal, which is a pair of file descriptors which are
 * indeed terminals.  Traditionally, we call those two elements of a pair a master and a slave end.
 * Everything written to one of the file descriptors appears on the other end as if it was typed on
 * a terminal. So if you write an interrupt character (usually Ctrl+C) to a master part of the pseudoterminal,
 * all the processes for which the slave part is a standard input will receive a
 * <a href="http://en.wikipedia.org/wiki/Unix_signal">SIGINT</a> signal.
 * This works both ways - anything typed on the slave part can be read by the master part. @n
 * So how can we use that facility? The basic plan is as follows:
 * -# Start the application and create a bidirectional communication channel; @n
 * Plain and simple just like
 * @ref TheNaiveApproach "above".
 * -#
 * -#
 * @section TheImplementation The improved implementation
 * @subsection TheNittyGrittyDetails The nitty-gritty details
 * @section TheResults Results
 * @section References
 * -# <tt><a href="http://man7.org/linux/man-pages/man1/script.1.html">script</a></tt>
 * -# <tt><a href="http://man7.org/linux/man-pages/man1/script.1.html">pty(7)</a></tt>
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#if defined __linux__
#include <pty.h>
#elif defined __FreeBSD__
#include <libutil.h>
#endif
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>

#include "event2/event.h"
#include "yanzc_buffer.h"
#include "yandu_log.h"

/**
 * @brief Size of the data buffer that stores
 * data to be sent to the child process.
 */
#define IO_TO_CHILD_BUFSIZE (32)

/**
 * @brief Size of the data buffer that stores
 * data received from the child process.
 */
#define IO_FROM_CHILD_BUFSIZE (4096)

static volatile sig_atomic_t quit = 0;
static const char file_name[] = "log_XXXXXX";

/**
 * @brief
 * @details
 * @param signal
 * @param info
 * @param context
 */
static void handle_child(int signal, siginfo_t *info, void *context) {
    LOG_DEBUG("%d %p %p", signal, info, context);
    /* Those declarations below fend off compiler warnings about unused variables */
    (void)(signal);
    (void)(info);
    (void)(context);
    quit = 1;
}

/**
 * @brief Master/slave communication routine.
 * @details
 * @param[in] fd_in - handle of the master part of the pseudo terminal. @n
 * We use this handle to read standard input of a child process and to write
 * to its standard output.
 * @return
 */
static int pass_all(int fd_in) {
    fd_set readset, readset_copy;
    fd_set writeset, writeset_copy;
    int maxfd;
    int fd_log;
    sigset_t blockset;

    char *log_file_name = strdup(file_name);
    fd_log = mkstemp(log_file_name);
    if (fd_log < 0 || evutil_make_socket_nonblocking(fd_log) < 0) {
        perror("mkstemp");
        return -1;
    }
    struct yanzc_buffer_t *io_buf_1 = io_buffer_new(IO_TO_CHILD_BUFSIZE);
    struct yanzc_buffer_t *io_buf_2 = io_buffer_new(IO_FROM_CHILD_BUFSIZE);

    struct yanz_read_slice_t io_buf_1_read_slice = io_buffer_get_read_slice(io_buf_1, 0);
    struct yanz_read_slice_t io_buf_2_read_slices[2] = {io_buffer_get_read_slice(io_buf_2, 0),
                                                        io_buffer_get_read_slice(io_buf_2, 0)};

    /* Set SIGCHLD handler */
    struct sigaction sa = {.sa_handler = NULL, .sa_flags = SA_SIGINFO};
    sa.sa_sigaction = handle_child;
    /* Don't block any signals during execution of SIGCHLD */
    sigemptyset(&sa.sa_mask);
    sigaction(SIGCHLD, &sa, NULL);

    /* SIGCHLD is still blocked */
    /* We set the signal mask to unblock SIGCHLD and then we pass it */
    /* to the pselect() call - this is the only way to reliably handle */
    /* SIGCHLD reception and test for it */
    sigemptyset(&blockset);
    sigfillset(&blockset);
    sigdelset(&blockset, SIGCHLD);

    LOG_DEBUG("%ld", (long int)quit);

    FD_ZERO(&readset);
    FD_ZERO(&readset_copy);
    FD_ZERO(&writeset);
    FD_ZERO(&writeset_copy);

    FD_SET(fd_in, &writeset_copy);
    FD_SET(fd_in, &readset_copy);
    FD_SET(STDIN_FILENO, &readset_copy);

    /* Main loop
     * We multiplex between a number of file descriptors:
     * - we read from the standard input and we pass it unaltered to the
     * master fd (passed as an argument to this function).
     * - we read form the master fd;
     * - whatever is read from the master fd is then logged to a file,
     * as well as written to the standard output.
     *
     * A child process gets a slave part of the same "pseudoterminal".
     * The slave part of the pseudo terminal pair is left in its default mode,
     * which is probably a good thing.
     */
    do {
        int result;
        /* Copy descriptor sets */
        memcpy(&readset, &readset_copy, sizeof(fd_set));
        memcpy(&writeset, &writeset_copy, sizeof(fd_set));
        /* Find the maximum fd */
        int idx;
        maxfd = 0;
        for (idx = 0; idx < FD_SETSIZE; ++idx) {
            if (FD_ISSET(idx, &writeset_copy) || FD_ISSET(idx, &readset_copy)) {
                if (maxfd < idx) {
                    maxfd = idx;
                }
            }
        }
        ++maxfd;

        /* Do the multiplexing */
        result = pselect(maxfd, &readset, &writeset, NULL, NULL, &blockset);
        if (result > 0) {
            /* Is there something to read from standard input? */
            if (FD_ISSET(STDIN_FILENO, &readset)) {
                /* Copy it to the buffer 1 */
                result = from_fd_to_buffer(STDIN_FILENO, io_buf_1);
                if (0 == result) {
                    /* Signal that buffer 1 can be copied:
                     * - to the log file
                     * - to the master part of the pseudo terminal.
                     */
                    FD_SET(fd_log, &writeset_copy);
                    FD_SET(fd_in, &writeset_copy);
                } else {
                    quit = 1;
                }
            }
            /* Can we write the child processe's terminal? */
            if (FD_ISSET(fd_in, &writeset)) {
                /* Copy data form the buffer 1 to this terminal */
                result = from_buffer_to_fd(&io_buf_1_read_slice, fd_in);
                if (0 == result) {
                    /* If all was written, then signal that we no longer need
                     * to write to the standard master terminal.
                     */
                    if (io_buf_1_read_slice.offset_read_ == io_buf_2->offset_write_) {
                        FD_CLR(fd_in, &writeset_copy);
                    }
                } else {
                    quit = 1;
                }
            }
            /* Is there something to read from child's processes terminal? */
            if (FD_ISSET(fd_in, &readset)) {
                /* Copy it to the buffer 2 */
                result = from_fd_to_buffer(fd_in, io_buf_2);
                if (0 == result) {
                    /* Signal that we need to write to the standard output */
                    FD_SET(STDOUT_FILENO, &writeset_copy);
                } else {
                    quit = 1;
                }
            }
            /* Can we write to the standard output? */
            if (FD_ISSET(STDOUT_FILENO, &writeset)) {
                /* Copy data form the buffer 2 to the standard output */
                result = from_buffer_to_fd(&io_buf_2_read_slices[0], STDOUT_FILENO);
                if (0 == result) {
                    if (io_buf_2_read_slices[0].offset_read_ == io_buf_2->offset_write_) {
                        /* If all was written, then signal that we no longer need
                         * to write to the standard output.
                         */
                        FD_CLR(STDOUT_FILENO, &writeset_copy);
                    }
                    FD_SET(fd_log, &writeset_copy);
                } else {
                    quit = 1;
                }
            }
            /* Can we write to the log file? */
            if (FD_ISSET(fd_log, &writeset)) {
                /* Copy data form the buffer 2 to the log file */
                result = from_buffer_to_fd(&io_buf_2_read_slices[1], fd_log);
                if (0 == result) {
                    if (io_buf_2_read_slices[1].offset_read_ == io_buf_2->offset_write_) {
                        /* If all was written, then signal that we no longer need
                         * to write to the log file.
                         */
                        FD_CLR(fd_log, &writeset_copy);
                    }
                } else {
                    quit = 1;
                }
            }
            io_buffer_realign(io_buf_1, &io_buf_1_read_slice, 1);
            io_buffer_realign(io_buf_2, io_buf_2_read_slices,
                              sizeof(io_buf_2_read_slices) / sizeof(io_buf_2_read_slices[0]));
        } else if (-1 == result) {
            if (EINTR == errno) {
                continue;
            } else {
                LOG_DEBUG("%d %s", errno, strerror(errno));
                break;
            }
        } else {
        }
    } while (0 == quit);
    fsync(fd_log);
    close(fd_log);
    free(io_buf_1);
    free(io_buf_2);
    free(log_file_name);
    LOG_DEBUG("%d", (int)quit);
    return -1;
}

/**
 * @brief Returns a valid shell executable name.
 * @details First, it takes a look at the @c SHELL environment variable.
 * If this one is not set, then it checks if there are some standard shell
 * executables accessible, i.e. it exists and it can be executed.
 * @return Returns a copy of the shell exectuable name it has found or the NULL
 * if there's no such executable found.
 */
char *get_shell_name(void) {
    const char *shell = getenv("SHELL");
    if (NULL == shell) {
        size_t idx;
        static const char *shell_candidates[] = {
            "/usr/local/bin/bash", "/usr/local/bin/sh", "/usr/local/bin/tcsh", "/bin/bash",
            "/bin/sh",             "/bin/ksh",          "/bin/tchs",
        };
        for (idx = 0; idx < sizeof(shell_candidates) / sizeof(shell_candidates[0]); ++idx) {
            if (0 == access(shell_candidates[idx], R_OK | X_OK)) {
                return strdup(shell_candidates[idx]);
            }
        }
    } else {
        return strdup(shell);
    }
    return NULL;
}

/**
 * @brief
 * @details
 * @param argc
 * @param argv
 * @param envp
 * @return
 */
int main(int argc, char *argv[], char *envp[]) {
    int master;
    (void)(argc);
    (void)(argv);
    struct termios stdin_data, stdin_data_copy;
    struct winsize win_size;
    sigset_t blockset, orig_set;

    if (!isatty(STDIN_FILENO) || !isatty(STDOUT_FILENO)) {
        perror("isatty");
        exit(EXIT_FAILURE);
    }
    LOG_DEBUG("%s", ttyname(STDIN_FILENO));

    if (0 != ioctl(STDIN_FILENO, TIOCGWINSZ, &win_size) || 0 != tcgetattr(STDIN_FILENO, &stdin_data)) {
        perror("ioctl || tcgetattr");
        exit(EXIT_FAILURE);
    }
    memcpy(&stdin_data_copy, &stdin_data, sizeof(struct termios));

    /*
     * Safety precaution.
     * Until the situation settles after a fork() routine is called.
     * we block SIGCHLD reception. We do restore the SIGCHLD handling
     * in both parent and a child.
     */
    sigemptyset(&blockset);
    sigaddset(&blockset, SIGCHLD);
    sigprocmask(SIG_BLOCK, &blockset, &orig_set);

    /*
     * Nobody knows how much space should be reserved for name.
     * Therefore, we pass NULL for the 'name' parameter, as this is the only secure value we can
     * pass. We also do not do anyting special about the slave part of the terminal,
     * we are quite happy with the default settings, hence NULL for 'term' parameter.
     */
    pid_t cpid = forkpty(&master, NULL, NULL, &win_size);
    if (0 == cpid) {
        /* In the child process */
        LOG_DEBUG("%s", ttyname(STDIN_FILENO));
        /* Restore original signal mask */
        sigprocmask(SIG_SETMASK, &orig_set, NULL);
        /* Execute the shell process. */
        char *shell = get_shell_name();
        if (NULL != shell) {
            LOG_DEBUG( "%s", shell);
            char *shell_argp[] = {shell, NULL};
            execve(shell, shell_argp, envp);
        }
        /* If we got that far, it means that execve() failed. We log an error and bail out */
        LOG_DEBUG("%d %s", errno, strerror(errno));
        exit(EXIT_FAILURE);
    } else if (cpid > 0) {
        /* In the parent process */
        int status;
        LOG_DEBUG("isatty(%d)=%d", master, isatty(master));
        cfmakeraw(&stdin_data);
        if (0 == tcsetattr(STDIN_FILENO, TCSANOW, &stdin_data)
            && 0 == evutil_make_socket_nonblocking(STDIN_FILENO)
            && 0 == evutil_make_socket_nonblocking(STDOUT_FILENO)
            && 0 == evutil_make_socket_nonblocking(master)) {
            pass_all(master);
            tcsetattr(STDIN_FILENO, TCSANOW, &stdin_data_copy);
            waitpid(cpid, &status, 0);
            exit(EXIT_SUCCESS);
        } else {
            perror("parent ");
            waitpid(cpid, &status, 0);
            exit(EXIT_FAILURE);
        }
    } else {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    return 0;
}
