#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <net/ethernet.h>
#include "include/dns.h"
#include "include/utils.h"
#include "include/applicationLayer.h"

#define HTTP_PORT 80
#define DNS_PORT 53

int IsHTTP(uint16_t sourcePort, uint16_t destinationPort) {
    return (sourcePort == HTTP_PORT || destinationPort == HTTP_PORT);
}

int IsDNS(uint16_t sourcePort, uint16_t destinationPort) {
    return (sourcePort == DNS_PORT || destinationPort == DNS_PORT);
}

int IsHTTPRequest(const uint8_t *tcpPayload, uint16_t tcpPayloadLen) {
    if (tcpPayloadLen < 4) {
        return 0;
    }

    if (memcmp(tcpPayload, "GET", 3) == 0 || memcmp(tcpPayload, "POST", 4) == 0 ||
        memcmp(tcpPayload, "PUT", 3) == 0 || memcmp(tcpPayload, "DELETE", 6) == 0) {
        printf("HTTP Request\n");
        return 1;
    }

    return 0;
}

int IsHTTPResponse(const uint8_t *tcpPayload, uint16_t tcpPayloadLen) {
    if (tcpPayloadLen < 4) {
        return 0;
    }

    if (memcmp(tcpPayload, "HTTP", 4) == 0) {
        printf("HTTP Response\n");
        return 1;
    }

    return 0;
}

void PrintInfoHTTP(uint16_t sourcePort, uint16_t destinationPort, struct tcphdr *tcpHeader, struct iphdr *ipHeader,
                   uint8_t *dataBuffer, uint16_t dataLength) {
    if (!IsHTTP(sourcePort, destinationPort)) return;
    const uint8_t *tcpPayload = (const uint8_t *)(dataBuffer + sizeof(struct ethhdr) + (ipHeader->ihl * 4) +
                                                  (tcpHeader->doff * 4));
    uint16_t tcpPayloadLen = dataLength - sizeof(struct ethhdr) - (ipHeader->ihl * 4) - (tcpHeader->doff * 4);
    if (tcpPayloadLen <= 0) return;
    if (IsHTTPRequest(tcpPayload, tcpPayloadLen)) {
        Dump(tcpPayload, tcpPayloadLen);
    } else if (IsHTTPResponse(tcpPayload, tcpPayloadLen)) {
        Dump(tcpPayload, tcpPayloadLen);
    }
}

void PrintInfoDNS(uint16_t sourcePort, uint16_t destinationPort, struct iphdr *ipHeader, uint8_t *dataBuffer) {
    if (!IsDNS(sourcePort, destinationPort)) return;
    const uint8_t *dnsData = (const uint8_t *) (dataBuffer + sizeof(struct ethhdr) + (ipHeader->ihl * 4) +
                                                 sizeof(struct udphdr));
    uint16_t flags = ntohs(*(uint16_t *) dnsData);

    dnsHeader *header = (dnsHeader *) dnsData;
    header->id = ntohs(header->id);
    header->flags = ntohs(header->flags);
    header->qdcount = ntohs(header->qdcount);
    header->ancount = ntohs(header->ancount);
    header->nscount = ntohs(header->nscount);
    header->arcount = ntohs(header->arcount);

    if ((flags & 0x8000) == 0) {
        printf("DNS Request:\n");
    } else {
        printf("DNS Response:\n");
    }
    printf("==header==\n");
    printf("ID: %u\n", header->id);
    printf("Flags: 0x%04X\n", header->flags);
    printf("Questions: %u\n", header->qdcount);
    printf("Answers: %u\n", header->ancount);
    printf("Authority Records: %u\n", header->nscount);
    printf("Additional Records: %u\n", header->arcount);
    printf("-----------------\n");
    if (header->qdcount > 0) {
        printf("==question==\n");
        const uint8_t *questionData = dnsData + sizeof(dnsHeader);

        for (int i = 0; i < header->qdcount; i++) {
            dnsQuestion *question = (dnsQuestion *) questionData;

            question->qtype = ntohs(question->qtype);
            question->qclass = ntohs(question->qclass);

            printf("Domain: %s\n", questionData + sizeof(dnsQuestion));
            printf("Type: %u\n", question->qtype);
            printf("Class: %u\n", question->qclass);

            questionData += sizeof(dnsQuestion) + strlen((const char *) questionData + sizeof(dnsQuestion)) + 1;
            printf("-----------------\n");
        }
    }
    if (header->ancount > 0) {
        printf("==answers==\n");

        const uint8_t *answer_data =
                dnsData + sizeof(dnsHeader) + header->qdcount * (sizeof(dnsQuestion) + 1);

        for (int i = 0; i < header->ancount; i++) {
            dnsAnswer *answer = (dnsAnswer *) answer_data;

            answer->type = ntohs(answer->type);
            answer->class = ntohs(answer->class);
            answer->ttl = ntohl(answer->ttl);
            answer->dataLength = ntohs(answer->dataLength);

            printf("Name: %s\n", answer_data + sizeof(dnsAnswer));
            printf("Type: %u\n", answer->type);
            printf("Class: %u\n", answer->class);
            printf("TTL: %u\n", answer->ttl);
            printf("Data Length: %u\n", answer->dataLength);

            answer_data += sizeof(dnsAnswer) + answer->dataLength;
            printf("-----------------\n");
        }
    }
    if (header->nscount > 0) {
        printf("==authority records==\n");

        const uint8_t *authority_data =
                dnsData + sizeof(dnsHeader) + header->qdcount * (sizeof(dnsQuestion) + 1) +
                header->ancount * (sizeof(dnsAnswer) + 1);

        for (int i = 0; i < header->nscount; i++) {
            dnsAuthority *authority = (dnsAuthority *) authority_data;

            authority->type = ntohs(authority->type);
            authority->class = ntohs(authority->class);
            authority->ttl = ntohl(authority->ttl);
            authority->dataLength = ntohs(authority->dataLength);

            printf("Name: %s\n", authority_data + sizeof(dnsAuthority));
            printf("Type: %u\n", authority->type);
            printf("Class: %u\n", authority->class);
            printf("TTL: %u\n", authority->ttl);
            printf("Data Length: %u\n", authority->dataLength);

            authority_data += sizeof(dnsAuthority) + authority->dataLength;
        }
        printf("-----------------\n");
    }
    if (header->arcount > 0) {
        printf("==additional records==\n");

        const uint8_t *additionalData =
                dnsData + sizeof(dnsHeader) + header->qdcount * (sizeof(dnsQuestion) + 1) +
                header->ancount * (sizeof(dnsAnswer) + 1) +
                header->nscount * (sizeof(dnsAuthority) + 1);

        for (int i = 0; i < header->arcount; i++) {
            dnsAdditional *additional = (dnsAdditional *) additionalData;

            additional->type = ntohs(additional->type);
            additional->class = ntohs(additional->class);
            additional->ttl = ntohl(additional->ttl);
            additional->dataLength = ntohs(additional->dataLength);

            printf("Name: %s\n", additionalData + sizeof(dnsAdditional));
            printf("Type: %u\n", additional->type);
            printf("Class: %u\n", additional->class);
            printf("TTL: %u\n", additional->ttl);
            printf("Data Length: %u\n", additional->dataLength);

            additionalData += sizeof(dnsAdditional) + additional->dataLength;
        }
        printf("-----------------\n");
    }
    printf("\n");
}