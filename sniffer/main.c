#include <time.h>
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
#include "include/erproc.h"

#define HTTP_PORT 80
#define DNS_PORT 53
#define BUF_SIZE 65536

int total_len = 0;
uint8_t data_buffer[BUF_SIZE];
struct iphdr *ip_header = NULL;

void process_packet(char **argv);

void dump(const uint8_t *data_buffer, uint data_len);

void printInfoHTTP(uint16_t source_port, struct tcphdr *tcp_header);

void printInfoDNS(uint16_t source_port, uint16_t destination_port);

void printInfoTCP(const char *source_ip, const char *destination_ip);

void printInfoUDP(const char *source_ip, const char *destination_ip);

void printInfoICMP(const char *source_ip, const char *destination_ip);

void dump(const uint8_t *data_buffer, uint data_len) {
    const char *http_data = (const char *) data_buffer;
    const char *end_of_http_data = http_data + data_len;

    while (http_data < end_of_http_data) {
        const char *end_of_line = strchr(http_data, '\n');
        if (end_of_line == NULL) {
            printf("%s\n", http_data);
            break;
        }

        size_t line_length = end_of_line - http_data;
        printf("%.*s\n", (int) line_length, http_data);

        http_data = end_of_line + 1;
    }
}

void printInfoHTTP(uint16_t source_port, struct tcphdr *tcp_header) {
    if (source_port == HTTP_PORT) {
        const uint8_t *http_data =
                (const uint8_t *) data_buffer + sizeof(struct ethhdr) + (ip_header->ihl * 4) + (tcp_header->doff * 4);
        uint http_data_len = total_len - sizeof(struct ethhdr) - (ip_header->ihl * 4) + (tcp_header->doff * 4);
        printf("HTTP data:\n");
        dump(http_data, http_data_len);
    }
}

void printInfoDNS(uint16_t source_port, uint16_t destination_port) {
    if (source_port == DNS_PORT || destination_port == DNS_PORT) {
        const uint8_t *dns_data =
                (const uint8_t *) data_buffer + sizeof(struct ethhdr) + (ip_header->ihl * 4) + sizeof(struct udphdr);
        uint dns_data_length = total_len - sizeof(struct ethhdr) - sizeof(struct iphdr) - sizeof(struct udphdr);
        uint16_t flags = ntohs(*(uint16_t *) dns_data);
        if ((flags & 0x8000) == 0) {
            printf("DNS Request:");
            dump(dns_data, dns_data_length);
        } else {
            printf("DNS Response:");
            dump(dns_data, dns_data_length);
        }
    }
}

void printInfoTCP(const char *source_ip, const char *destination_ip) {
    struct tcphdr *tcp_header = (struct tcphdr *) (data_buffer + sizeof(struct ethhdr) + (ip_header->ihl * 4));
    uint16_t source_port = ntohs(tcp_header->source);
    uint16_t destination_port = ntohs(tcp_header->dest);
    printf("TCP, %s.%u > %s.%u\n", source_ip, source_port, destination_ip, destination_port);
    printInfoHTTP(source_port, tcp_header);
}

void printInfoUDP(const char *source_ip, const char *destination_ip) {
    struct udphdr *udp_header = (struct udphdr *) (data_buffer + sizeof(struct ethhdr) + (ip_header->ihl * 4));
    uint16_t source_port = ntohs(udp_header->source);
    uint16_t destination_port = ntohs(udp_header->dest);
    printf("UDP, %s.%u > %s.%u\n", source_ip, source_port, destination_ip, destination_port);
    printInfoDNS(source_port, destination_port);
}

void printInfoICMP(const char *source_ip, const char *destination_ip) {
    struct icmphdr *icmp_header = (struct icmphdr *) (data_buffer + sizeof(struct ethhdr) + (ip_header->ihl * 4));
    printf("ICMP, %s > %s\n", source_ip, destination_ip);
    printf("ICMP Type: %u\n", icmp_header->type);
    printf("ICMP Code: %u\n", icmp_header->code);
}

void process_packet(char **argv) {
    // IP info
    ip_header = (struct iphdr *) (data_buffer + sizeof(struct ethhdr));
    uint version = ip_header->version;
    uint8_t protocol = ip_header->protocol;
    uint16_t total_len = ntohs(ip_header->tot_len);
    uint8_t ttl = ip_header->ttl;
    char source_ip[INET_ADDRSTRLEN];
    char destination_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ip_header->saddr, source_ip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &ip_header->daddr, destination_ip, INET_ADDRSTRLEN);

    // current time
    time_t current_time = time(NULL);
    struct tm *local_time = localtime(&current_time);
    char date_string[64];
    strftime(date_string, sizeof(date_string), "%Y-%m-%d", local_time);

    if ((strcmp(argv[1], "TCP") == 0 && protocol == IPPROTO_TCP) ||
        (strcmp(argv[1], "UDP") == 0 && protocol == IPPROTO_UDP) ||
        (strcmp(argv[1], "ICMP") == 0 && protocol == IPPROTO_ICMP) ||
        (strcmp(argv[1], "ALL") == 0)) {
        printf("Packet info (len: %u): %s, IPv%d, ttl %d, ", total_len, date_string, version, ttl);
    }
    if (strcmp(argv[1], "ALL") == 0) {
        switch (protocol) {
            case IPPROTO_TCP:
                printInfoTCP(source_ip, destination_ip);
                break;
            case IPPROTO_UDP:
                printInfoUDP(source_ip, destination_ip);
                break;
            case IPPROTO_ICMP:
                printInfoICMP(source_ip, destination_ip);
                break;
            default:
                printf("Unknown Protocol\n");
                break;
        }
    } else if (strcmp(argv[1], "TCP") == 0 && protocol == IPPROTO_TCP) {
        printInfoTCP(source_ip, destination_ip);
    } else if (strcmp(argv[1], "UDP") == 0 && protocol == IPPROTO_UDP) {
        printInfoUDP(source_ip, destination_ip);
    } else if (strcmp(argv[1], "ICMP") == 0 && protocol == IPPROTO_ICMP) {
        printInfoICMP(source_ip, destination_ip);
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: sudo ./program <protocol>\n");
        return 0;
    }
    if (strcmp(argv[1], "TCP") != 0 && strcmp(argv[1], "UDP") != 0 && strcmp(argv[1], "ICMP") != 0 &&
        strcmp(argv[1], "ALL") != 0) {
        printf("Flag not found\n");
        return 0;
    }
    int fd;
    fd = Socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    while (1) {
        total_len = Recv(fd, data_buffer, BUF_SIZE, 0);
        struct ethhdr *eth_header = (struct ethhdr *) data_buffer;
        uint16_t protocol = ntohs(eth_header->h_proto);
        if (protocol != ETH_P_IP) continue;
        process_packet(argv);
    }
}