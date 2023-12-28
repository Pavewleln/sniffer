#include <stdio.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <net/ethernet.h>
#include <netinet/ip_icmp.h>
#include "include/transportLayer.h"
#include "include/applicationLayer.h"

void PrintInfoTCP(const char *sourceIp, const char *destinationIp, struct iphdr *ipHeader, uint8_t *dataBuffer,
             const uint dataLength) {
    struct tcphdr *tcpHeader = (struct tcphdr *) (dataBuffer + sizeof(struct ethhdr) + (ipHeader->ihl * 4));
    uint16_t sourcePort = ntohs(tcpHeader->source);
    uint16_t destinationPort = ntohs(tcpHeader->dest);
    printf("TCP, %s.%u > %s.%u\n", sourceIp, sourcePort, destinationIp, destinationPort);
    PrintInfoHTTP(sourcePort, destinationPort, ipHeader, tcpHeader, dataBuffer, dataLength);
}

void PrintInfoUDP(const char *sourceIp, const char *destinationIp, struct iphdr *ipHeader, uint8_t *dataBuffer,
             const uint dataLength) {
    struct udphdr *udpHeader = (struct udphdr *) (dataBuffer + sizeof(struct ethhdr) + (ipHeader->ihl * 4));
    uint16_t sourcePort = ntohs(udpHeader->source);
    uint16_t destinationPort = ntohs(udpHeader->dest);
    printf("UDP, %s.%u > %s.%u\n", sourceIp, sourcePort, destinationIp, destinationPort);
    PrintInfoDNS(sourcePort, destinationPort, ipHeader, dataBuffer, dataLength, sizeof(struct udphdr));
}

void PrintInfoICMP(const char *sourceIp, const char *destinationIp, struct iphdr *ipHeader, uint8_t *dataBuffer) {
    struct icmphdr *icmpHeader = (struct icmphdr *) (dataBuffer + sizeof(struct ethhdr) + (ipHeader->ihl * 4));
    printf("ICMP, %s > %s\n", sourceIp, destinationIp);
    printf("ICMP Type: %u\n", icmpHeader->type);
    printf("ICMP Code: %u\n", icmpHeader->code);
}