#include <sys/types.h>
#include <sys/socket.h>
#include "arpa/inet.h"
#include "./include/erproc.h"
#include "./include/help.h"

int Socket(int domain, int type, int protocol) {
    const int server = socket(domain, type, protocol);
    if (server == -1) {
        Die("Socket failed\n");
    }
    return server;
}

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int server = bind(sockfd, addr, addrlen);
    if (server == -1) {
        Die("Bind failed\n");
    }
}

void Listen(int sockfd, int backlog) {
    int server = listen(sockfd, backlog);
    if (server == -1) {
        Die("Listen failed\n");
    }
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int server = accept(sockfd, addr, addrlen);
    if (server == -1) {
        Die("Accept failed\n");
    }
}

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int res = connect(sockfd, addr, addrlen);
    if (res == -1) {
        Die("connect failed\n");
    }
    return res;
}

int Inet_pton(int af, const char *src, void *dst) {
    int res = inet_pton(af, src, dst);
    if (res == 0) {
        Die("inet_pton failed: src does not contain a character\n");
    }
    if (res == -1) {
        Die("inet_pton failed\n");
    }
    return res;
};

ssize_t Recv_from(int sockfd, void *buf, size_t len, int flags,
                  struct sockaddr *src_addr, socklen_t *addrlen) {
    ssize_t recv_len = recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
    if (recv_len == -1) {
        Die("Recv_from failed\n");
    }
    return recv_len;
}

ssize_t Send_to(int sockfd, const void *buf, size_t len, int flags,
                const struct sockaddr *dest_addr, socklen_t addrlen) {
    ssize_t send_len = sendto(sockfd, buf, len, flags,dest_addr, addrlen);
    if (send_len == -1) {
        Die("Send_to failed\n");
    }
    return send_len;
}