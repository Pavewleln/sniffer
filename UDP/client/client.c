#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../include/erproc.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 65535
#define BUFFER_SIZE 1024

int main(int argc, char** argv) {
    int sockfd;
    struct sockaddr_in server_addr;
    socklen_t server_addr_len;
    char buffer[BUFFER_SIZE];

    // Создание сокета
    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    // Настройка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    printf("Enter message: ");
    fgets(buffer, BUFFER_SIZE, stdin);

    // Отправка данных серверу
    ssize_t send_len = Send_to(sockfd, buffer, strlen(buffer), 0,
                              (struct sockaddr*)&server_addr, sizeof(server_addr));

    //Ожидание ответа от сервера
    server_addr_len = sizeof(server_addr);
    ssize_t recv_len = Recv_from(sockfd, buffer, BUFFER_SIZE - 1, 0,
                                (struct sockaddr*)&server_addr, &server_addr_len);

    buffer[recv_len] = '\0';
    printf("Получен ответ от сервера: %s\n", buffer);

    // Закрытие сокета
    close(sockfd);

    return 0;
}