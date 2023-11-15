#include <stdio.h>
#include <stdint.h>
#include "include/erproc.h"

int Socket(int domain, int type, int protocol) {
    int server = socket(domain, type, protocol);
    Die(server, "socket failed");
    return server;
}

int Recv(int sockfd, void *buf, size_t len, int flags) {
    int total_len = recv(sockfd, buf, len, flags);
    Die(total_len, "Recv failed");
    return total_len;
}