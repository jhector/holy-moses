#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#define LOG_STREAM stdout

static int32_t log_fd = -1;

void log_file(char *msg)
{
    if (log_fd < 0) {
        log_fd = open("/tmp/saloon.log", O_WRONLY | O_APPEND | O_CREAT);
    }

    if (log_fd < 0)
        return;

    write(log_fd, msg, strlen(msg));
}

void log_msg(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vfprintf(LOG_STREAM, fmt, args);
}

void die(char *error)
{
    log_msg("FATAL: %s\n", error);
    exit(-1);
}

void _write(const char *out)
{
    write(1, out, strlen(out));
}

void menu()
{
    _write("Welcome to the Holy Moses invite system.\n\n");

    _write("(1) enter referal\n");
    _write("(2) request invite\n");
    _write("(3) quit\n\n");
}

int32_t main()
{
    char input[10];
    uint32_t choice = 0;

    menu();

    while (1) {
        _write("Your choice: ");

        bzero(input, sizeof(input));
        if (read(0, input, sizeof(input)-1) <= 0)
            break;

        if (!sscanf(input, "%d", &choice))
            continue;

        switch (choice) {
        case 1:
            break;
        case 2:
            break;
        case 3:
            goto leave;
        }
    }

leave:
    if (log_fd > 0)
        close(log_fd);
    return 0;
}
