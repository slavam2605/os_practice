#include "../lib/bufio.h"

#define BUF_SIZE 1024

int main(int argc, char** argv)
{
    struct buf_t * buf = buf_new(BUF_SIZE);
    ssize_t res;
    while ((res = buf_fill(STDIN_FILENO, buf, BUF_SIZE)) > 0)
        buf_flush(STDOUT_FILENO, buf, BUF_SIZE);
    buf_flush(STDOUT_FILENO, buf, BUF_SIZE);
}
