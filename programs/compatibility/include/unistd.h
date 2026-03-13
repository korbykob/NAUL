#pragma once

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

static int isatty(int fd)
{
    return fd == STDIN_FILENO || fd == STDOUT_FILENO || fd == STDERR_FILENO;
}
