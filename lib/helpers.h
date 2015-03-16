#ifndef HELPERS_H
#define HELPERS_H

#include <unistd.h>

ssize_t read_(int fd, void* buffer, size_t count);
ssize_t write_(int fd, const void* buffer, size_t count);
ssize_t read_until(int fd, void* buffer, size_t count, char delimiter);

#endif
