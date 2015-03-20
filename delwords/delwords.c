#include "../lib/helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF_SIZE 10

int prefix(char* s, char* p)
{
    while (*s != 0 && *p != 0) {
        if (*s != *p)
            return 0;
        s++;
        p++;
    }
    if (*p == 0)
        return 2;
    return 1;
}

int main(int argc, char** argv) 
{
    char* pattern = argv[1];
    char* a = (char*) malloc(MAX_BUF_SIZE);
    int res, res2;
    int len;
    int p_len = strlen(pattern);
    while ((res = read_(STDIN_FILENO, a, MAX_BUF_SIZE)) > 0) {
        while (1) {
            len = 0;    
            while ((res2 = prefix(a + len, pattern)) == 0)
                len++;
            if (write_(STDOUT_FILENO, a, len) < 0) {
                perror("error: ");
                return 0;
            }
            a = a + len;
            if (res2 != 2)
                break;
            else
                a = a + p_len;
        }
    }
    if (res < 0) 
        perror("error: ");
    return 0;
}
