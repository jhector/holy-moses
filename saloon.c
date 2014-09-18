#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#define REF_SIZE 64
#define LOG_STREAM stdout

struct request {
    const char *name;
    uint64_t age;
};

static int32_t log_fd = -1;

struct request last_requests[20];
uint32_t idx = 0;

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

int32_t check_referal(const char *code)
{
    char ref[REF_SIZE] = {0};

    int32_t fd = open("referals", O_RDONLY);
    if (fd < 0)
        return 0;

    if (read(fd, ref, 12) <= 0)
        goto fail_close;

    if (!strcmp(code, ref)) {
        close(fd);
        return 1;
    } else {
        log_file("[!] invalid referal code\n");
    }

fail_close:
    close(fd);
    return 0;
}

void enter_referal()
{
    char code[REF_SIZE] = {0};

    _write("Code (12 bytes): ");

    if (read(0, code, sizeof(code)-1) < 0)
        die("reading referal code");

    if (check_referal(code)) {
        log_msg("Your code: %s, is valid\nPlease contact your referal\n", code);
    } else {
        log_msg("Invalid code: %s\n", code);
    }
}

void store_last(const char *name, uint64_t age)
{
    last_requests[idx % 20].name = name;
    last_requests[idx % 20].age = age;
    idx++;
}

int32_t check_request(const char *name, uint64_t age)
{
    char msg[50] = {0};

    snprintf(msg, 22 + strlen(name), "Blocked request from %s", name);

    log_file(msg);

    store_last(name, age);
    return 0;
}

void request_invite()
{
    char buf[28];
    uint64_t age;
    char name[28];

    _write("Age: ");
    if (read(0, buf, sizeof(name)-1) < 0)
        return;

    age = strtoull(buf, NULL, 10);

    _write("Name: ");
    if (read(0, name, sizeof(name)-1) < 0)
        return;

    if (check_request(name, age)) {
        log_msg("Thank you %sYour invitation will be mailed to you.\n", name);
    } else {
        log_msg("We are sorry %sThere is no more room.\n", name);
    }
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
            enter_referal();
            break;
        case 2:
            request_invite();
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
