#include "../lib/bufio.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <fcntl.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>

#define BUF_SIZE 65536

int get_socket(const char * port) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sock, s;
    
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    hints.ai_flags = AI_PASSIVE;     // suitable for bind(2)ing a socket 
                                     // that will accept(2) connections (c) man 3 getaddrinfo
    hints.ai_protocol = IPPROTO_TCP; // IP protocol
   
    s = getaddrinfo(NULL, port, &hints, &result);
    if (s != 0) 
        return 1;

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock == -1)
            continue;

        int one;
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) == -1)
            return 1;

        if (bind(sock, rp->ai_addr, rp->ai_addrlen) == 0)
            break;

        close(sock);
    }
    
    if (rp == NULL) 
        return 1;
        
    freeaddrinfo(result); 
    return sock;
}

int main(int argc, char* argv[]) {
    struct sigaction act;
    memset(&act, '\0', sizeof(act));
    act.sa_handler = SIG_IGN;  // do not create zombies
    act.sa_flags = SA_RESTART; // restart restartable syscalls
   
    if (sigaction(SIGCHLD, &act, NULL) < 0)
        return 1;
    
    int sock1 = get_socket(argv[1]);
    int sock2 = get_socket(argv[2]);
  
    struct buf_t * buf = buf_new(BUF_SIZE);

    if (listen(sock1, -1) == -1 || listen(sock2, -1) == -1)
        return 1;
    
    struct sockaddr_in client1;
    struct sockaddr_in client2;
    socklen_t sz1 = sizeof(client1);
    socklen_t sz2 = sizeof(client2);
   
    while (1) { 
        int fd1 = accept(sock1, (struct sockaddr*)&client1, &sz1);
        if (fd1 == -1)
            return 1;
        int fd2 = accept(sock2, (struct sockaddr*)&client2, &sz2);
        if (fd2 == -1)
            return 1;
   
        if (!(fork())) {    
            while (buf_fill(fd1, buf, 1) > 0) 
                buf_flush(fd2, buf, buf->size);
            _exit(0);
        }
        
        if (!(fork())) {    
            while (buf_fill(fd2, buf, 1) > 0) 
                buf_flush(fd1, buf, buf->size);
            _exit(0);
        }

        close(fd1);
        close(fd2);
    }

    close(sock1);
    close(sock2);
    return 0;
}
