#include "../lib/helpers.h"
#include "../lib/bufio.h"
#include <string.h>
#include <signal.h>
#include <sys/types.h>

#define errcall(func) if (func < 0) return 1;

#define BUF_SIZE 2048

int space_or_delim(char c, char delim) {
    return c == delim || c == '\0' || c == ' ' || c == '\t';
}

int count_lexem(char* buffer, char delim) {
    if (buffer[0] == '\0')
        return 0;
    int count = 0;
    int i = 1;
    while (1) {
        if (!space_or_delim(buffer[i - 1], delim) 
            && space_or_delim(buffer[i], delim))
            count++;
        if (buffer[i] == '\0' || buffer[i] == delim)
            return count;
        i++;
    }
}

char* get_lexem(char* buffer, char* delim, int* new_pos) {
    int i = 0;
    while (buffer[i] == ' ' || buffer[i] == '\t')
        i++;
    int start = i;
    while (buffer[i] != ' ' && buffer[i] != '\t' && buffer[i] != '\0' && buffer[i] != *delim)
        i++;
    int end = i;
    *delim = buffer[i];
    *new_pos = end;
    if (start == end)
        return 0;
    return strndup(buffer + start, end - start); 
}

int get_delim(char* buffer, char delim) {
    int i = 0; 
    while (buffer[i] == ' ' || buffer[i] == '\t')
        i++;
    if (buffer[i] == delim)
        i++;
    return i;
}

void sigint_handler(int sig) {}

#define NO_DELIM_FOUND -1
#define READ_UNTIL_EOF -2
#define READ_ERROR     -3

int main() {

    struct sigaction act;
    memset(&act, '\0', sizeof(act));
    act.sa_handler = &sigint_handler;
   
    if (sigaction(SIGINT, &act, NULL) < 0)
        return 1;
 
    struct buf_t* buf = buf_new(BUF_SIZE); 
    while (1) {
        errcall(write_(STDOUT_FILENO, "$ ", 2));
        int pos = buf_readuntil(STDIN_FILENO, buf, '\n');
        if (pos == READ_UNTIL_EOF)
            return 0;
        if (pos == READ_ERROR) {
            errcall(write_(STDOUT_FILENO, "\n$ ", 1));
            buf->size = 0;
            continue;
        }
        char* buffer = buf->buffer;        
        buffer[pos] = '\0';
        struct execargs_t* arguments[1024];
        int k = 0;
        while (1) {
            char delim;
            int argc = count_lexem(buffer, '|');
            if (argc == 0)
                break;
            char* argv[argc]; 
            int shift;
            for (int i = 0; i < argc; i++) {
                delim = '|';
                argv[i] = get_lexem(buffer, &delim, &shift);
                buffer += shift;
            }
            arguments[k] = (struct execargs_t*) malloc(sizeof(struct execargs_t));
            *arguments[k] = create_args(argc, argv);  
            shift = get_delim(buffer, '|'); 
            buffer += shift; // skip delim
            k++;
        }
        buf->size -= (buffer - buf->buffer + 1);
        runpiped(arguments, k);
    }
    return 0;
}
