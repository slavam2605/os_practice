#define _GNU_SOURCE
#include "helpers.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

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

#define EXEC_FAILED -1
#define WAIT_FAILED -2
#define EXIT_FAILED -3

int spawn(const char* file, char* const argv[])
{
    pid_t child_pid;
    if (!(child_pid = fork()))
        if (execvp(file, argv) < 0)
            return EXEC_FAILED;
    int ret_code;
    if (waitpid(child_pid, &ret_code, 0) < 0)
        return WAIT_FAILED; // emm, what should I do?
    if (WIFEXITED(ret_code)) 
        return WEXITSTATUS(ret_code);
    else 
        return EXIT_FAILED; 
}

struct execargs_t create_args(int argc, char** argv)
{
    struct execargs_t result;
    result.argv = (char**) malloc((argc + 1) * sizeof(char*));
    int i;
    for (i = 0; i < argc; i++) 
        result.argv[i] = strdup(argv[i]); 
    result.argv[argc] = NULL;
    return result;
}

int exec(struct execargs_t* args) {
    if (spawn(args->argv[0], args->argv) == EXEC_FAILED)
        return -1;
    return 0;
}
 
int childn;
int* childa;

void sig_handler(int sig) {
    for (int i = 0; i < childn; i++) 
        kill(childa[i], SIGKILL);
    childn = 0;
}

int runpiped(struct execargs_t** programs, size_t n) {
	if (n == 0)
        return 0;
    int pipefd[n - 1][2];
	int child[n];	       
	for (int i = 0; i < n - 1; i++) 
        if (pipe2(pipefd[i], O_CLOEXEC) < 0)
            return -1;
	for (int i = 0; i < n; i++) {
		if (!(child[i] = fork())) {
            if (i != 0)
				dup2(pipefd[i - 1][0], STDIN_FILENO);
			if (i != n - 1)
				dup2(pipefd[i][1], STDOUT_FILENO);
			_exit(execvp(programs[i]->argv[0], programs[i]->argv));	
		}
        if (child[i] == -1)
            return -1;
	}
	for (int i = 0; i < n - 1; i++) {
		close(pipefd[i][0]);
		close(pipefd[i][1]);
	}
    
    childn = n;
    childa = (int*) child;
    struct sigaction act;
    memset(&act, '\0', sizeof(act));
    act.sa_handler = &sig_handler;
   
    if (sigaction(SIGINT, &act, NULL) < 0) 
        return -1;

	int status;
	for (int i = 0; i < n; i++) 
        waitpid(child[i], &status, 0);
    childn = 0;
    return 0;
}
