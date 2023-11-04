#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#define BUF_SIZE 65536

void dump(const uint8_t *data_buffer, const uint length);

void process_packet(const uint8_t *data_buffer, const uint length);

int Socket(int domain, int type, int protocol);

int main() {
    uint8_t buffer[BUF_SIZE];
    int recv_length, fd;
    fd = Socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    int packets = 5;
    for (int i = 0; i < packets; i++) {
        recv_length = recv(fd, buffer, BUF_SIZE, 0);
        printf("Got a %d byte packet: \n", recv_length);
        process_packet(buffer, recv_length);
        printf("\n");
    }
    return 0;
}

void process_packet(const uint8_t *data_buffer, const uint length) {
    // Получаем указатели IP и TCP
    struct iphdr *ip_header = (struct iphdr *) data_buffer;
    struct tcphdr *tcp_header = (struct tcphdr *) (data_buffer + sizeof(struct iphdr));

    // Получаем информацию заголовков IP и TCP
    uint8_t source_ip[INET_ADDRSTRLEN];
    uint8_t destination_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ip_header->saddr, source_ip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &ip_header->daddr, destination_ip, INET_ADDRSTRLEN);
    uint16_t source_port = ntohs(tcp_header->source);
    uint16_t destination_port = ntohs(tcp_header->dest);

    // Выводим информацию
    printf("Packet info: \n");
    printf("IP source: %s\n", source_ip);
    printf("IP destination: %s\n", destination_ip);
    printf("Port source: %u\n", source_port);
    printf("Port destination: %u\n", destination_port);

    printf("Byte representation: \n");
    dump(data_buffer, length);

}

void dump(const uint8_t *data_buffer, const uint length) {
    for (uint i = 0; i < length; i++) {
        uint byte = data_buffer[i];
        printf("%02x ", byte);
        if (((i % 16) == 15) || (i == length - 1)) {
            for (uint j = 0; j < 15 - (i % 16); j++) {
                printf(" ");
            }
            printf("| ");
            for (uint j = i - (i % 16); j <= i; j++) {
                byte = data_buffer[j];
                if ((byte > 31) && byte < 127) {
                    printf("%c", byte);
                } else {
                    printf(".");
                }
            }
            printf("\n");
        }
    }
}

int Socket(int domain, int type, int protocol) {
    const int server = socket(domain, type, protocol);
    if (server == -1) {
        perror("socket failed");
    }
    return server;
}
