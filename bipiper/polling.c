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
#include <poll.h>
#include <errno.h>

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

#define FD_COUNT 256

int main(int argc, char* argv[]) {

    struct pollfd fds[FD_COUNT];
    memset(&fds, '\0', sizeof(struct pollfd) * FD_COUNT); // all fds[i].event = 0, so ignored in poll

    struct sigaction act;
    memset(&act, '\0', sizeof(act));
    act.sa_handler = SIG_IGN;  // do not create zombies
    act.sa_flags = SA_RESTART; // restart restartable syscalls
   
    if (sigaction(SIGCHLD, &act, NULL) < 0)
        return 1;
    
    int sock1 = get_socket(argv[1]);
    int sock2 = get_socket(argv[2]);
  
    if (listen(sock1, -1) == -1 || listen(sock2, -1) == -1)
        return 1;
    
    struct sockaddr_in client1;
    struct sockaddr_in client2;
    socklen_t sz1 = sizeof(client1);
    socklen_t sz2 = sizeof(client2);
  
    int count = 0;
    struct buf_t * buf1[127];
    struct buf_t * buf2[127];
    fds[0].fd = sock1;
    fds[0].events = POLLIN;
    fds[1].fd = sock2;
    fds[1].events = POLLIN;
   
    int state = 0;
    int fd1, fd2;

    while (1) { 
        int res = poll(fds, 2 + 2 * count, -1);
        if (res <= 0 && errno != EINTR)
            return 1;

        for (int i = 0; i < count; i++) {         
            if (fds[2 + 2 * i].revents & POLLRDHUP ||
                fds[2 + 2 * i + 1].revents & POLLRDHUP) {

                close(fds[2 + 2 * i].fd);
                close(fds[2 + 2 * i + 1].fd);
                buf_free(buf1[i]);
                buf_free(buf2[i]);
                buf1[i] = NULL;
                buf2[i] = NULL;
                
                struct pollfd t = fds[2 + 2 * i];
                fds[2 + 2 * i] = fds[2 + 2 * count - 2];
                fds[2 + 2 * count - 2] = t;
                t = fds[2 + 2 * i + 1];
                fds[2 + 2 * i + 1] = fds[2 + 2 * count - 1];
                fds[2 + 2 * count - 1] = t;
                struct buf_t * tt = buf1[i];
                buf1[i] = buf1[count - 1];
                buf1[count - 1] = tt;
                tt = buf2[i];
                buf2[i] = buf2[count - 1];
                buf2[count - 1] = tt;

                count--;
                fds[0].events |= POLLIN;
                fds[1].events |= POLLIN;
                break;
            }
            if (fds[2 + 2 * i].revents & POLLIN) {
                buf_fill(fds[2 + 2 * i].fd, buf1[i], 1);
                fds[2 + 2 * i + 1].events |= POLLOUT;
                if (buf1[i]->size == buf1[i]->capacity)
                    fds[2 + 2 * i].events &= ~POLLIN;
            }
            if (fds[2 + 2 * i + 1].revents & POLLIN) {
                buf_fill(fds[2 + 2 * i + 1].fd, buf2[i], 1);
                fds[2 + 2 * i].events |= POLLOUT;
                if (buf2[i]->size == buf2[i]->capacity)
                    fds[2 + 2 * i + 1].events &= ~POLLIN;
            }
            if (fds[2 + 2 * i].revents & POLLOUT) {
                buf_flush(fds[2 + 2 * i].fd, buf2[i], 1);
                fds[2 + 2 * i + 1].events |= POLLIN;
                if (buf2[i]->size == 0)
                    fds[2 + 2 * i].events &= ~POLLOUT;
            }
            if (fds[2 + 2 * i + 1].revents & POLLOUT) {
                buf_flush(fds[2 + 2 * i + 1].fd, buf1[i], 1);
                fds[2 + 2 * i].events |= POLLIN;
                if (buf1[i]->size == 0)
                    fds[2 + 2 * i + 1].events &= ~POLLOUT;
            }            
        }

        if (state == 0 && fds[0].revents & POLLIN) {        
            fd1 = accept(sock1, (struct sockaddr*)&client1, &sz1);
            if (fd1 == -1)
                return 1;
            state = 1;
        }
        if (state == 1 && fds[1].revents & POLLIN) {
            fd2 = accept(sock2, (struct sockaddr*)&client2, &sz2);
            if (fd2 == -1)
                return 1;
            state = 0;
            fds[2 + 2 * count].fd = fd1;
            fds[2 + 2 * count].events = POLLIN | POLLRDHUP;
            fds[2 + 2 * count + 1].fd = fd2;
            fds[2 + 2 * count + 1].events = POLLIN | POLLRDHUP;
            buf1[count] = buf_new(BUF_SIZE);
            buf2[count] = buf_new(BUF_SIZE);
            count++;
            if (count >= 127) {
                fds[0].events &= ~POLLIN;
                fds[1].events &= ~POLLIN;
            }
        }
   
    }

    close(sock1);
    close(sock2);
    return 0;
}
