#include "../lib/bufio.h"

#define BUF_SIZE 1024

int main(int argc, char** argv)
{
    struct buf_t * buf = buf_new(BUF_SIZE);
    ssize_t res, res2, res3;
    while ((res = buf_fill(STDIN_FILENO, buf, 1)) > 0)
        res2 = buf_flush(STDOUT_FILENO, buf, buf->size);
    res3 = buf_flush(STDOUT_FILENO, buf, buf->size);
    if (res < 0 || res2 < 0 || res3 < 0)
        return -1;
    return 0;
}
