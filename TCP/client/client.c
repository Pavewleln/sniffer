#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/erproc.h"

#define MAX_LINE 1024

int main() {
    int server_host = 0;
    char *server_send_buf = (char *) malloc(sizeof(char) * MAX_LINE);
    int fd = Socket(AF_INET, SOL_SOCKET, 0);


    // server_host
    printf("Please, enter the server host in the correct format: ");
    scanf("%d", &server_host);
    if (!server_host) {
        printf("server host: wrong format\n");
    }

    // server send buf
    printf("Please, enter what you want to send to the server: ");
    scanf("%s", server_send_buf);

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(server_host);

    Inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    Connect(fd, (struct sockaddr *) &addr, sizeof(addr));
    write(fd, server_send_buf, MAX_LINE);
    free(server_send_buf);
    close(fd);
    return 0;
}