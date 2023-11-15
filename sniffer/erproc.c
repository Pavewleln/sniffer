#include <sys/types.h>
#include <sys/socket.h>
#include "arpa/inet.h"
#include "include/erproc.h"
#include "include/help.h"

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