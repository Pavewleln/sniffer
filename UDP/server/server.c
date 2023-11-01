#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "../include/erproc.h"

#define SERVER_PORT 65535
#define BUF_SIZE 1024

int main(int argc, char **argv) {
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;
    char buffer[BUF_SIZE];

    // Настройка адреса сервера

    int server = Socket(AF_INET, SOCK_DGRAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    Bind(server, (struct  sockaddr*)&server_addr, sizeof(server_addr));

    printf("UDP server started and expected message\n");

    while(1){
        client_addr_len = sizeof(client_addr);
        ssize_t recv_len = Recv_from(server, buffer, BUF_SIZE - 1, 0, (struct sockaddr*)&client_addr, &client_addr_len);
        buffer[recv_len] = '\0';
        printf("%s\n", buffer);

        const char* response = "Hello, client!";
        ssize_t send_len = Send_to(server, response, strlen(response), 0,
                                  (struct sockaddr*)&client_addr, client_addr_len);
        printf("Response send client succefully\n");
    }


    close(server);

    return 0;
}