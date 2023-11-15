#include <sys/socket.h>
#include "include/erproc.h"
#include <assert.h>

int Socket(int domain, int type, int protocol) {
    int server = socket(domain, type, protocol);
    assert(server != -1);
    return server;
}

int Recv(int sockfd, void *buf, size_t len, int flags) {
    int total_len = recv(sockfd, buf, len, flags);
    assert(total_len != -1);
    return total_len;
}