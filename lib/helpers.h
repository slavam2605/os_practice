#ifndef HELPERS_H
#define HELPERS_H

#include <unistd.h>

struct execargs_t {
    char** argv;
};

ssize_t read_(int fd, void* buffer, size_t count);
ssize_t write_(int fd, const void* buffer, size_t count);
ssize_t read_until(int fd, void* buffer, size_t count, char delimiter);
int spawn(const char* file, char* const argv[]);

struct execargs_t create_args(int argc, char** argv);
int exec(struct execargs_t* args);
int runpiped(struct execargs_t** programs, size_t n);

#endif
