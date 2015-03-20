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
    char* s  = (char*) malloc(MAX_BUF_SIZE);
    int res, res2, res3;
    int len;
    int p_len = strlen(pattern);
    char* a = s;
    char* b = pattern;
    char* cc = (char*) malloc(strlen(pattern));
    strcpy(cc, pattern);
    printf("lol\n");
    int rest_c = 0;
    int init_len = strlen(pattern);
    while ((res = read_(STDIN_FILENO, a, MAX_BUF_SIZE)) > 0) {
        while (1) {
            len = 0;    
            while ((res2 = prefix(a + len, pattern)) == 0) {
                len++;
                res--;
                write_(STDOUT_FILENO, cc, rest_c);
                rest_c = 0;
                pattern = b;
                p_len = init_len;
            }
            if (res3 = write_(STDOUT_FILENO, a, len) < 0) {
                perror("error: ");
                return res3;
            }
            a = a + len;
            if (res2 == 2) { 
                a = a + p_len;
                rest_c = 0;
                pattern = b;
                p_len = init_len;
            }
            if (a == s + MAX_BUF_SIZE || res2 == 1) {
                int c_len = MAX_BUF_SIZE - (a - s);
                a = s;
                int i;
                for (i = 0; i < c_len; i++) {
                    cc[i] = pattern[i];
                }
                rest_c = c_len;
                pattern += c_len;
                p_len = strlen(pattern);
                break;
            }
            if (res2 != 2)
                break;
        }
    }
    if (res < 0) 
        perror("error: ");
    return res;
}
