#include <sys/socket.h>
#include "include/erproc.h"
#include "include/utils.h"

int Socket(int domain, int type, int protocol) {
    int socketResult = socket(domain, type, protocol);
    IsError(socketResult, "socket failed\n");
    return socketResult;
}

ssize_t Recv(int sockfd, void *buf, size_t len, int flags) {
    ssize_t recvResult = recv(sockfd, buf, len, flags);
    IsError( recvResult, "recv failed\n");
    return recvResult;
}