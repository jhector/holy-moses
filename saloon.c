#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>

void log_msg(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vprintf(fmt, args);
}

void die(char *error)
{
    log_msg("Fatal error: %s\n", error);
    exit(-1);
}

int32_t main()
{
    return 0;
}
