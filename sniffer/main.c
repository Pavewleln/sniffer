#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <net/ethernet.h>

#define BUF_SIZE 65536

void dump(const uint8_t *data_buffer, const uint length);

void process_packet(const uint8_t *data_buffer, const uint length);

void httpProtocol(uint16_t source_port, uint16_t destination_port, void *transport_header, uint16_t total_len);

void dnsProtocol(uint16_t source_port, uint16_t destination_port, void *transport_header, uint16_t total_len);

void portInfoTCP(void *transport_header, uint16_t total_len);

void portInfoUDP(void *transport_header, uint16_t total_len);

void portInfoICMP(void *transport_header);

int Socket(int domain, int type, int protocol);

int main() {
    uint8_t buffer[BUF_SIZE];
    int recv_length, fd;
    fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (fd < 0) {
        perror("socket");
        return 1;
    }
    while (1) {
        recv_length = recv(fd, buffer, BUF_SIZE, 0);

        struct ethhdr *eth_header = (struct ethhdr *) buffer;
        uint16_t protocol = ntohs(eth_header->h_proto);

        if (protocol == ETH_P_IP) {
            sleep(1);
            process_packet(buffer, recv_length);
            printf("\n");
        }
    }
}

void process_packet(const uint8_t *data_buffer, const uint length) {
    // Получаем указатели IP и TCP/UDP/ICMP
    struct iphdr *ip_header = (struct iphdr *) (data_buffer + sizeof(struct ethhdr));
    void *transport_header = (void *) (ip_header + sizeof(struct iphdr));

    // Получаем информацию заголовка IP
    char source_ip[INET_ADDRSTRLEN];
    char destination_ip[INET_ADDRSTRLEN];
    uint16_t total_len = ntohs(ip_header->tot_len);
    inet_ntop(AF_INET, &ip_header->saddr, source_ip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &ip_header->daddr, destination_ip, INET_ADDRSTRLEN);

    // Выводим информацию
    printf("Packet info:\n");
    printf("IP source: %s\n", source_ip);
    printf("IP destination: %s\n", destination_ip);
    printf("Total len: %d\n", total_len);

    if (ip_header->protocol == IPPROTO_TCP) {
        portInfoTCP(transport_header, total_len);
    } else if (ip_header->protocol == IPPROTO_UDP) {
        portInfoUDP(transport_header, total_len);
    } else if (ip_header->protocol == IPPROTO_ICMP) {
        portInfoICMP(transport_header);
    } else {
        printf("Unknown Protocol\n");
    }

    printf("Byte representation:\n");
    dump((data_buffer + sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(transport_header)), length);
}

void portInfoTCP(void *transport_header, uint16_t total_len) {
    struct tcphdr *tcp_header = (struct tcphdr *) transport_header;
    uint16_t source_port = ntohs(tcp_header->source);
    uint16_t destination_port = ntohs(tcp_header->dest);
    printf("TCP Port source: %u\n", source_port);
    printf("TCP Port destination: %u\n", destination_port);
    httpProtocol(source_port, destination_port, transport_header, total_len);
    dnsProtocol(source_port, destination_port, transport_header, total_len);
}

void portInfoUDP(void *transport_header, uint16_t total_len) {
    struct udphdr *udp_header = (struct udphdr *) transport_header;
    uint16_t source_port = ntohs(udp_header->source);
    uint16_t destination_port = ntohs(udp_header->dest);
    printf("UDP Port source: %u\n", source_port);
    printf("UDP Port destination: %u\n", destination_port);
    httpProtocol(source_port, destination_port, transport_header, total_len);
    dnsProtocol(source_port, destination_port, transport_header, total_len);
}

void portInfoICMP(void *transport_header) {
    // icmp - прокотол сети для обработки ошибок
    struct icmphdr *icmp_header = (struct icmphdr *) transport_header;
    printf("ICMP Type: %u\n", icmp_header->type);
    printf("ICMP Code: %u\n", icmp_header->code);
}

void httpProtocol(uint16_t source_port, uint16_t destination_port, void *transport_header, uint16_t total_len) {
    if (source_port == 80 || destination_port == 80) {
        const uint8_t *http_data = (const uint8_t *) transport_header + sizeof(struct tcphdr);
        uint http_data_len = total_len - sizeof(struct ethhdr) - sizeof(struct iphdr) - sizeof(struct tcphdr);
        printf("HTTP data:\n");
        dump(http_data, http_data_len);
    }
}

void dnsProtocol(uint16_t source_port, uint16_t destination_port, void *transport_header, uint16_t total_len) {
    if (source_port == 53 || destination_port == 53) {
        const uint8_t *dns_data = (const uint8_t *) transport_header + sizeof(struct udphdr);
        uint dns_data_length = total_len - sizeof(struct iphdr) - sizeof(struct udphdr);
        uint16_t flags = ntohs(*(uint16_t *) dns_data);
        if ((flags & 0x8000) == 0) {
            printf("DNS Request:\n");
            dump(dns_data, dns_data_length);
        } else {
            printf("DNS Response:\n");
            dump(dns_data, dns_data_length);
        }
    }
}

void dump(const uint8_t *data_buffer, const uint length) {
    for (uint i = 0; i < length; i++) {
        uint byte = data_buffer[i];
        printf("%02x ", byte);
        if (((i % 16) == 15) || (i == length - 1)) {
            for (uint j = 0; j < 15 - (i % 16); j++) {
                printf("   ");
            }
            printf("| ");
            for (uint j = i - (i % 16); j <= i; j++) {
                uint byte = data_buffer[j];
                if ((byte > 31) && (byte < 127)) {
                    printf("%c", byte);
                } else {
                    printf(".");
                }
            }
            printf("\n");
        }
    }
    printf("=======================================================");
}

int Socket(int domain, int type, int protocol) {
    const int server = socket(domain, type, protocol);
    if (server == -1) {
        perror("socket failed");
    }
    return server;
}