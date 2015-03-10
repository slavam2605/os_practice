#include "../lib/helpers.h"
#include <stdio.h>

#define MAX_BUF_SIZE 1024

int main() 
{
    char a[MAX_BUF_SIZE];
    int res;
    while ((res = read_(STDIN_FILENO, a, MAX_BUF_SIZE)) > 0) {
        if (write_(STDOUT_FILENO, a, res) < 0) 
            return -1;
    }
    return res;
}
