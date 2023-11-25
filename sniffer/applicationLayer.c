#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <net/ethernet.h>
#include "include/dns.h"
#include "include/httpOutput.h"
#include "include/applicationLayer.h"

#define HTTP_PORT 80
#define DNS_PORT 53

void PrintInfoHTTP(uint16_t sourcePort, uint16_t destinationPort, struct tcphdr *tcpHeader, struct iphdr *ipHeader,
                   uint8_t *dataBuffer, uint16_t totalLen) {
    if (sourcePort == HTTP_PORT || destinationPort == HTTP_PORT) {
        const uint8_t *httpData =
                (const uint8_t *) (dataBuffer + sizeof(struct ethhdr) + (ipHeader->ihl * 4) + (tcpHeader->doff * 4));
        uint httpDataLen = totalLen - sizeof(struct ethhdr) - (ipHeader->ihl * 4) - (tcpHeader->doff * 4);
        if (httpDataLen <= 0) return;
        if (tcpHeader->syn && !tcpHeader->ack) {
            printHttpRequest(httpData, httpDataLen);
        } else {
            printHttpResponse(httpData, httpDataLen);
        }
    }
}

void PrintInfoDNS(uint16_t sourcePort, uint16_t destinationPort, struct iphdr *ipHeader, uint8_t *dataBuffer) {
    if (sourcePort == DNS_PORT || destinationPort == DNS_PORT) {
        const uint8_t *dns_data = (const uint8_t *) (dataBuffer + sizeof(struct ethhdr) + (ipHeader->ihl * 4) +
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
                answer->dataLength = ntohs(answer->dataLength);

                printf("Name: %s\n", answer_data + sizeof(dns_answer));
                printf("Type: %u\n", answer->type);
                printf("Class: %u\n", answer->class);
                printf("TTL: %u\n", answer->ttl);
                printf("Data Length: %u\n", answer->dataLength);

                answer_data += sizeof(dns_answer) + answer->dataLength;
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
                authority->dataLength = ntohs(authority->dataLength);

                printf("Name: %s\n", authority_data + sizeof(dns_authority));
                printf("Type: %u\n", authority->type);
                printf("Class: %u\n", authority->class);
                printf("TTL: %u\n", authority->ttl);
                printf("Data Length: %u\n", authority->dataLength);

                authority_data += sizeof(dns_authority) + authority->dataLength;
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
                additional->dataLength = ntohs(additional->dataLength);

                printf("Name: %s\n", additional_data + sizeof(dns_additional));
                printf("Type: %u\n", additional->type);
                printf("Class: %u\n", additional->class);
                printf("TTL: %u\n", additional->ttl);
                printf("Data Length: %u\n", additional->dataLength);

                additional_data += sizeof(dns_additional) + additional->dataLength;
            }
            printf("-----------------\n");
        }
        printf("\n");
    }
}