#include "../lib/helpers.h"
#include <string.h>
#include <stdio.h>

#define MAX_LINE_SIZE 1024

int main(int argc, char** argv)
{
    char a[MAX_LINE_SIZE];
    char s[MAX_LINE_SIZE + 1];
    char* args[argc + 1];
    int i;
    int res;
    int rc;
    while ((res = read_until(STDIN_FILENO, a, MAX_LINE_SIZE, '\n')) > 0) {
        strncpy(s, a, res);
        if (s[res - 1] == '\n')
            s[res - 1] = '\0';
        else
            s[res] = '\0';
        for (i = 1; i < argc; i++)
            args[i - 1] = argv[i];
        args[argc - 1] = s;
        args[argc] = NULL;
        if ((rc = spawn(argv[1], args)) == 0)
            write_(STDOUT_FILENO, a, res);
    }
    if (res < 0) {
        perror("error: ");
        return res;
    }
    return 0;
}
