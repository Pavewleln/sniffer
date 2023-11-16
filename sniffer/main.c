#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
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
#include "include/dns.h"
#include "include/main.h"
#include "include/http_output.h"

#define HTTP_PORT 80
#define DNS_PORT 53
#define BUF_SIZE 65536

uint total_len = 0;
uint8_t *data_buffer = NULL;
struct iphdr *ip_header = NULL;

void printInfoHTTP(uint16_t source_port, uint16_t destination_port, struct tcphdr *tcp_header) {
    if (source_port == HTTP_PORT || destination_port == HTTP_PORT) {
        const uint8_t *http_data =
                (const uint8_t *) (data_buffer + sizeof(struct ethhdr) + (ip_header->ihl * 4) + (tcp_header->doff * 4));
        uint http_data_len = total_len - sizeof(struct ethhdr) - (ip_header->ihl * 4) - (tcp_header->doff * 4);
        if (http_data_len <= 0) return;
        if (tcp_header->syn && !tcp_header->ack) {
            printHttpRequest(http_data, http_data_len);
        } else {
            printHttpResponse(http_data, http_data_len);
        }
    }
}

void printInfoDNS(uint16_t source_port, uint16_t destination_port) {
    if (source_port == DNS_PORT || destination_port == DNS_PORT) {
        const uint8_t *dns_data = (const uint8_t *) (data_buffer + sizeof(struct ethhdr) + (ip_header->ihl * 4) +
                                                     sizeof(struct udphdr));
        uint16_t flags = ntohs(*(uint16_t *) dns_data);

        dns_header *dnsHeader = (dns_header *) dns_data;
        dnsHeader->id = ntohs(dnsHeader->id);
        dnsHeader->flags = ntohs(dnsHeader->flags);
        dnsHeader->qdcount = ntohs(dnsHeader->qdcount);
        dnsHeader->ancount = ntohs(dnsHeader->ancount);
        dnsHeader->nscount = ntohs(dnsHeader->nscount);
        dnsHeader->arcount = ntohs(dnsHeader->arcount);

        if ((flags & 0x8000) == 0) {
            printf("DNS Request:\n");
        } else {
            printf("DNS Response:\n");
        }
        printf("==header==\n");
        printf("ID: %u\n", dnsHeader->id);
        printf("Flags: 0x%04X\n", dnsHeader->flags);
        printf("Questions: %u\n", dnsHeader->qdcount);
        printf("Answers: %u\n", dnsHeader->ancount);
        printf("Authority Records: %u\n", dnsHeader->nscount);
        printf("Additional Records: %u\n", dnsHeader->arcount);
        printf("-----------------\n");
        if (dnsHeader->qdcount > 0) {
            printf("==question==\n");
            const uint8_t *question_data = dns_data + sizeof(dns_header);

            for (int i = 0; i < dnsHeader->qdcount; i++) {
                dns_question *question = (dns_question *) question_data;

                question->qtype = ntohs(question->qtype);
                question->qclass = ntohs(question->qclass);

                printf("Domain: %s\n", question_data + sizeof(dns_question));
                printf("Type: %u\n", question->qtype);
                printf("Class: %u\n", question->qclass);

                question_data += sizeof(dns_question) + strlen((const char *) question_data + sizeof(dns_question)) + 1;
                printf("-----------------\n");
            }
        }
        if (dnsHeader->ancount > 0) {
            printf("==answers==\n");

            const uint8_t *answer_data =
                    dns_data + sizeof(dns_header) + dnsHeader->qdcount * (sizeof(dns_question) + 1);

            for (int i = 0; i < dnsHeader->ancount; i++) {
                dns_answer *answer = (dns_answer *) answer_data;

                answer->type = ntohs(answer->type);
                answer->class = ntohs(answer->class);
                answer->ttl = ntohl(answer->ttl);
                answer->data_length = ntohs(answer->data_length);

                printf("Name: %s\n", answer_data + sizeof(dns_answer));
                printf("Type: %u\n", answer->type);
                printf("Class: %u\n", answer->class);
                printf("TTL: %u\n", answer->ttl);
                printf("Data Length: %u\n", answer->data_length);

                answer_data += sizeof(dns_answer) + answer->data_length;
                printf("-----------------\n");
            }
        }
        if (dnsHeader->nscount > 0) {
            printf("==authority records==\n");

            const uint8_t *authority_data =
                    dns_data + sizeof(dns_header) + dnsHeader->qdcount * (sizeof(dns_question) + 1) +
                    dnsHeader->ancount * (sizeof(dns_answer) + 1);

            for (int i = 0; i < dnsHeader->nscount; i++) {
                dns_authority *authority = (dns_authority *) authority_data;

                authority->type = ntohs(authority->type);
                authority->class = ntohs(authority->class);
                authority->ttl = ntohl(authority->ttl);
                authority->data_length = ntohs(authority->data_length);

                printf("Name: %s\n", authority_data + sizeof(dns_authority));
                printf("Type: %u\n", authority->type);
                printf("Class: %u\n", authority->class);
                printf("TTL: %u\n", authority->ttl);
                printf("Data Length: %u\n", authority->data_length);

                authority_data += sizeof(dns_authority) + authority->data_length;
            }
            printf("-----------------\n");
        }
        if (dnsHeader->arcount > 0) {
            printf("==additional records==\n");

            const uint8_t *additional_data =
                    dns_data + sizeof(dns_header) + dnsHeader->qdcount * (sizeof(dns_question) + 1) +
                    dnsHeader->ancount * (sizeof(dns_answer) + 1) +
                    dnsHeader->nscount * (sizeof(dns_authority) + 1);

            for (int i = 0; i < dnsHeader->arcount; i++) {
                dns_additional *additional = (dns_additional *) additional_data;

                additional->type = ntohs(additional->type);
                additional->class = ntohs(additional->class);
                additional->ttl = ntohl(additional->ttl);
                additional->data_length = ntohs(additional->data_length);

                printf("Name: %s\n", additional_data + sizeof(dns_additional));
                printf("Type: %u\n", additional->type);
                printf("Class: %u\n", additional->class);
                printf("TTL: %u\n", additional->ttl);
                printf("Data Length: %u\n", additional->data_length);

                additional_data += sizeof(dns_additional) + additional->data_length;
            }
            printf("-----------------\n");
        }
        printf("\n");
    }
}

void printInfoTCP(const char *source_ip, const char *destination_ip) {
    struct tcphdr *tcp_header = (struct tcphdr *) (data_buffer + sizeof(struct ethhdr) + (ip_header->ihl * 4));
    uint16_t source_port = ntohs(tcp_header->source);
    uint16_t destination_port = ntohs(tcp_header->dest);
    printf("TCP, %s.%u > %s.%u\n", source_ip, source_port, destination_ip, destination_port);
    printInfoHTTP(source_port, destination_port, tcp_header);
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
        printf("Usage: sudo ./program <protocol>(TCP/UDP/ICMP/ALL)\n");
        return 0;
    }
    if (strcmp(argv[1], "TCP") != 0 && strcmp(argv[1], "UDP") != 0 && strcmp(argv[1], "ICMP") != 0 &&
        strcmp(argv[1], "ALL") != 0) {
        printf("Flag not found\n");
        return 0;
    }
    int fd;
    fd = Socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    data_buffer = (unsigned char *) malloc(65536);
    while (1) {
        total_len = Recv(fd, data_buffer, BUF_SIZE, 0);
        struct ethhdr *eth_header = (struct ethhdr *) data_buffer;
        uint16_t protocol = ntohs(eth_header->h_proto);
        if (protocol != ETH_P_IP) continue;
        process_packet(argv);
    }
}