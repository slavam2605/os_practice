#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

ssize_t read_(int fd, void* buffer, size_t count) 
{
    ssize_t res;
    size_t rest = count;
    while ((res = read(fd, buffer, rest)) > 0) {
        buffer += res;
        rest -= res;
    }
    if (res < 0)
        return res;
    return count - rest;
}

ssize_t write_(int fd, const void* buffer, size_t count) 
{
    ssize_t res;
    size_t rest = count;
    while ((res = write(fd, buffer, rest)) > 0) {
        buffer += res;
        rest -= res;
    }
    if (res < 0)
        return res;
    return count - rest;
}


ssize_t read_until(int fd, void* buffer, size_t count, char delimiter) 
{
    ssize_t res;
    size_t rest = count;
    while ((res = read(fd, buffer, 1)) > 0) {
        if (*((char*) buffer) == delimiter)
            return count - rest + 1;
        buffer += res;
        rest -= res;
    }
    if (res < 0)
        return res;
    return count - rest;
}

int spawn(const char* file, char* const argv[])
{
    pid_t child_pid;
    if (!(child_pid = fork()))
        execvp(file, argv);
    int ret_code;
    if (waitpid(child_pid, &ret_code, 0) < 0)
        return -1; // emm, what should I do?
    if (WIFEXITED(ret_code)) 
        return WEXITSTATUS(ret_code);
    else 
        return -1; 
}
