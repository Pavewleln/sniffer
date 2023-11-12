#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "../include/erproc.h"
#include "../include/help.h"

#define MAX_LINE 1024

int main(int argc, char **argv) {
    const int server = Socket(AF_INET, SOCK_STREAM, 0);
    char *file_name = (char *) malloc(15 * sizeof(char));
    if (argv[1] == NULL) {
        printf("Enter output file: ");
        scanf("%s", file_name);
    } else {
        strcpy(file_name, argv[1]);
    }



//    write(STDOUT_FILENO, file_name, strlen(file_name));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(65535);
    socklen_t addrlen = sizeof addr;
    Bind(server, (struct sockaddr *) &addr, addrlen);
    Listen(server, 5);
    int fd = Accept(server, (struct sockaddr *) &addr, &addrlen);
    char *buf = (char *) malloc(MAX_LINE * sizeof(char));
    ssize_t nread = read(fd, buf, MAX_LINE);
    if (nread == -1) {
        Die("Read failed\n");
    }
    if (nread == 0) {
        printf("END on FILE occured\n");
    }
    int buflen = strlen(buf);
    if (buflen > 0) {
        buf[buflen] = '\n';
    }
    fopen(file_name, "a+");
    int file = open(file_name, O_WRONLY);
    write(file, buf, buflen);
    char *response = "Hello, client";
    write(fd, response, nread);

    sleep(1);
    free(file_name);
    close(fd);
    close(server);

    return 0;
}
