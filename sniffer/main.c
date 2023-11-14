#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <net/ethernet.h>

#define HTTP_PORT 80
#define DNS_PORT 53
#define BUF_SIZE 65536

void dump(const uint8_t *data_buffer, uint http_data_len);

void process_packet();

void printInfoHTTP(uint16_t source_port, uint16_t destination_port, struct tcphdr *tcp_header);

void printInfoDNS(uint16_t source_port, uint16_t destination_port);

void printInfoTCP();

void printInfoUDP();

void printInfoICMP();

int Socket(int domain, int type, int protocol);

int total_len = 0;
uint8_t data_buffer[BUF_SIZE];
struct iphdr *ip_header = NULL;

void checkError(int result, const char *message) {
    if (result == -1) {
        perror(message);
        exit(EXIT_FAILURE);
    }
}

int main() {
    int fd;
    fd = Socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    while (1) {
        total_len = recv(fd, data_buffer, BUF_SIZE, 0);
        checkError(total_len, "recv failed");

        struct ethhdr *eth_header = (struct ethhdr *) data_buffer;
        uint16_t protocol = ntohs(eth_header->h_proto);
        if (protocol == ETH_P_IP) {
            process_packet();
            printf("\n");
        }
    }
}

void process_packet() {
    ip_header = (struct iphdr *) (data_buffer + sizeof(struct ethhdr));

    char source_ip[INET_ADDRSTRLEN];
    char destination_ip[INET_ADDRSTRLEN];
    uint16_t total_len = ntohs(ip_header->tot_len);
    inet_ntop(AF_INET, &ip_header->saddr, source_ip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &ip_header->daddr, destination_ip, INET_ADDRSTRLEN);

    printf("Packet info:\n");
    printf("IP source: %s\n", source_ip);
    printf("IP destination: %s\n", destination_ip);
    printf("Total len: %d\n", total_len);

    switch (ip_header->protocol) {
        case IPPROTO_TCP:
            printInfoTCP();
            break;
        case IPPROTO_UDP:
            printInfoUDP();
            break;
        case IPPROTO_ICMP:
            printInfoICMP();
            break;
        default:
            printf("Unknown Protocol\n");
            break;
    }

//    dump((data_buffer + sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(transport_header)), length);
}

void printInfoTCP() {
    struct tcphdr *tcp_header = (struct tcphdr *) data_buffer + sizeof(struct ethhdr) + (ip_header->ihl * 4);
    uint16_t source_port = ntohs(tcp_header->source);
    uint16_t destination_port = ntohs(tcp_header->dest);
    printf("TCP Port source: %u\n", source_port);
    printf("TCP Port destination: %u\n", destination_port);
    printInfoHTTP(source_port, destination_port, tcp_header);
//    printInfoDNS(source_port, destination_port, transport_header, total_len);
}

void printInfoUDP() {
    struct udphdr *udp_header = (struct udphdr *) data_buffer + sizeof(struct ethhdr) + (ip_header->ihl * 4);
    uint16_t source_port = ntohs(udp_header->source);
    uint16_t destination_port = ntohs(udp_header->dest);
    printf("UDP Port source: %u\n", source_port);
    printf("UDP Port destination: %u\n", destination_port);
//    printInfoDNS(source_port, destination_port, transport_header, total_len);
}

void printInfoICMP() {
    struct icmphdr *icmp_header = (struct icmphdr *) data_buffer + sizeof(struct ethhdr) + (ip_header->ihl * 4);
    printf("ICMP Type: %u\n", icmp_header->type);
    printf("ICMP Code: %u\n", icmp_header->code);
}

void printInfoHTTP(uint16_t source_port, uint16_t destination_port, struct tcphdr *tcp_header) {
    if (source_port == HTTP_PORT || destination_port == HTTP_PORT) {
        const uint8_t *http_data =
                (const uint8_t *) data_buffer + sizeof(struct ethhdr) + (ip_header->ihl * 4) + (tcp_header->doff * 4);
        uint http_data_len = total_len - sizeof(struct ethhdr) - sizeof(struct iphdr) - sizeof(struct tcphdr);
        printf("HTTP data:\n");
        dump(http_data, http_data_len);
        exit(EXIT_FAILURE);
    }
}

void printInfoDNS(uint16_t source_port, uint16_t destination_port) {
    if (source_port == DNS_PORT || destination_port == DNS_PORT) {
//        const uint8_t *dns_data = (const uint8_t *) data_buffer + sizeof(struct ethhdr) + (ip_header->ihl * 4) + sizeof(struct udphdr);
//        uint16_t flags = ntohs(*(uint16_t *) dns_data);
//        if ((flags & 0x8000) == 0) {
//            printf("DNS Request:");
//            dump(dns_data, dns_data_length);
//        } else {
//            printf("DNS Response:");
//            dump(dns_data, dns_data_length);
//        }
    }
}

void dump(const uint8_t *data_buffer, uint http_data_len) {
    fwrite(data_buffer, sizeof(uint8_t), http_data_len, stdout);
    printf("\n");
}

int Socket(int domain, int type, int protocol) {
    int server = socket(domain, type, protocol);
    checkError(server, "socket failed");
    return server;
}