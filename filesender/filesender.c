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
#include <stdio.h>

#define BUF_SIZE 65536

int main(int argc, char* argv[]) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sock, s;
    
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;  
    hints.ai_protocol = IPPROTO_TCP; // tcp
    
    s = getaddrinfo(NULL, argv[1], &hints, &result);
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
  
    struct buf_t * buf = buf_new(BUF_SIZE);

    if (listen(sock, -1) == -1)
        return 1;
    
    struct sockaddr_in client;
    socklen_t sz = sizeof(client);
   
    while (1) { 
        int fd = accept(sock, (struct sockaddr*)&client, &sz);
        if (fd == -1)
            return 1;
    
        if (!(fork())) {    
            int file = open(argv[2], 0);
            if (file < 0)
                return 1;

            while (buf_fill(file, buf, buf->capacity) > 0) 
                buf_flush(fd, buf, buf->size);
            close(fd);
            _exit(0);
        }
        close(fd);
    }

    close(sock);
    return 0;
}
