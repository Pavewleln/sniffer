#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <net/ethernet.h>
#include "include/dns.h"
#include "include/utils.h"
#include "include/applicationLayer.h"

#define DNS_FLAG_RESPONSE 0x8000

static int IsHTTPPacket(const uint8_t *tcpPayload, const uint tcpPayloadLen) {
    if (tcpPayloadLen < 4) return 0;

    if (memcmp(tcpPayload, "GET", 3) == 0 || memcmp(tcpPayload, "POST", 4) == 0 ||
        memcmp(tcpPayload, "PUT", 3) == 0 || memcmp(tcpPayload, "DELETE", 6) == 0 ||
        memcmp(tcpPayload, "OPTIONS", 7) == 0 || memcmp(tcpPayload, "HEAD", 4) == 0 ||
        memcmp(tcpPayload, "PATCH", 5) == 0) {
        printf("HTTP Request\n");
        return 1;
    } else if (memcmp(tcpPayload, "HTTP", 4) == 0) {
        printf("HTTP Response\n");
        return 1;
    }

    return 0;
}

void PrintInfoHTTP(struct iphdr *ipHeader, struct tcphdr *tcpHeader, uint8_t *dataBuffer, const uint dataLength) {

    const uint8_t *httpData = (const uint8_t *) (dataBuffer + sizeof(struct ethhdr) + (ipHeader->ihl * 4) +
                                                 (tcpHeader->doff * 4));
    const uint httpDataLength = dataLength - sizeof(struct ethhdr) - (ipHeader->ihl * 4) - (tcpHeader->doff * 4);

    if (IsHTTPPacket(httpData, dataLength)) Dump(httpData, httpDataLength);

    // If is not response and is not request, this is not http packet
    return;
}

void PrintInfoDNS(struct iphdr *ipHeader, uint8_t *dataBuffer, const uint dataLength, size_t transportHdr) {

    const uint8_t *dnsData = (const uint8_t *) (dataBuffer + sizeof(struct ethhdr) + (ipHeader->ihl * 4) +
                                                transportHdr);

    dnsHeader *header = (dnsHeader *) dnsData;

    // If qdcount is 0, then the packet is not a valid DNS packet.
    if (ntohs(header->qdcount) <= 0) return;

    const uint dnsDataLength = dataLength - sizeof(struct ethhdr) - (ipHeader->ihl * 4) - transportHdr;

    uint16_t flags = ntohs(*(uint16_t *) dnsData);
    if ((flags & DNS_FLAG_RESPONSE) == 0) {
        printf("DNS Request:\n");
    } else {
        printf("DNS Response:\n");
    }
    header->id = ntohs(header->id);
    header->flags = ntohs(header->flags);
    header->qdcount = ntohs(header->qdcount);
    header->ancount = ntohs(header->ancount);
    header->nscount = ntohs(header->nscount);
    header->arcount = ntohs(header->arcount);

    printf("Transaction ID: %u\n", header->id);
    printf("Flags: 0x%04X\n", header->flags);
    printf("Questions: %u\n", header->qdcount);
    printf("Answer RRs: %u\n", header->ancount);
    printf("Authority RRs: %u\n", header->nscount);
    printf("Additional RRs: %u\n", header->arcount);
    const uint8_t *questionData = dnsData + sizeof(dnsHeader);

    char name[256];
    int nameIndex = 0;
    int isCompressed = 0;
    int offset = 0;

    for (int i = 0; i < header->qdcount; i++) {
        // Variables for reading labels
        int labelLength = 0;
        const uint8_t *labelData = questionData + offset;

        // Checking for name compression
        if ((labelData[0] & 0xC0) == 0xC0) {
            // Compressed name
            isCompressed = 1;
            offset += 2;
            continue;
        }

        // Reading each label
        while (labelData[labelLength] != 0) {
            if (labelLength > 0) {
                name[nameIndex++] = '.';
            }
            int len = labelData[labelLength++];
            for (int j = 0; j < len; j++) {
                name[nameIndex++] = labelData[labelLength++];
            }
        }

        if (!isCompressed) {
            offset += labelLength + 1;
        }

        dnsQuestion *question = (dnsQuestion *) (questionData + offset);
        question->qtype = ntohs(question->qtype);
        question->qclass = ntohs(question->qclass);
        printf("Name: %s, type: %u, class: %u\n", name, question->qtype, question->qclass);
    }

    Dump(dnsData, dnsDataLength);
}