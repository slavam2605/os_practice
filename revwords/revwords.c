#include "../lib/helpers.h"
#include <stdio.h>

#define MAX_BUF_SIZE 4097

void reverse(char* s, int count) 
{
    int i = 0; 
    int j = count - 1;
    char t;
    while (i < j) {
        t = s[i];
        s[i] = s[j];
        s[j] = t;
        i++;
        j--;
    }
}

int main() 
{
    char a[MAX_BUF_SIZE];
    int res;
    while ((res = read_until(STDIN_FILENO, a, MAX_BUF_SIZE, ' ')) > 0) {
        reverse(a, res);
        a[res] = ' ';
        if (write_(STDOUT_FILENO, a, res + 1) < 0) {
            perror("error: ");
            return 0;
        }
    }
    if (res < 0) 
        perror("error: ");
    return 0;
}
