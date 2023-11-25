#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/ethernet.h>
#include "include/utils.h"
#include "include/erproc.h"
#include "include/transportLayer.h"

#define BUF_SIZE 65536

void ProcessPacket(uint8_t *dataBuffer);

void ProcessPacket(uint8_t *dataBuffer) {
    // IP info
    struct iphdr *ipHeader = (struct iphdr *) (dataBuffer + sizeof(struct ethhdr));
    uint version = ipHeader->version;
    uint8_t protocol = ipHeader->protocol;
    uint16_t totalLen = ntohs(ipHeader->tot_len);
    uint8_t ttl = ipHeader->ttl;
    char sourceIp[INET_ADDRSTRLEN];
    char destinationIp[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ipHeader->saddr, sourceIp, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &ipHeader->daddr, destinationIp, INET_ADDRSTRLEN);

    char dateString[64];
    GetCurrentDate(dateString, sizeof(dateString));
    printf("Packet info (len: %u): %s, IPv%d, ttl %d, ", totalLen, dateString, version, ttl);

    switch (protocol) {
        case IPPROTO_TCP:
            PrintInfoTCP(sourceIp, destinationIp, ipHeader, dataBuffer, totalLen);
            break;
        case IPPROTO_UDP:
            PrintInfoUDP(sourceIp, destinationIp, ipHeader, dataBuffer);
            break;
        case IPPROTO_ICMP:
            PrintInfoICMP(sourceIp, destinationIp, ipHeader, dataBuffer);
            break;
        default:
            printf("Unknown Protocol\n");
            break;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: sudo ./program <protocol>(TCP/UDP/ICMP/ALL)\n");
        return 0;
    }
    int protocolFlag = -1;
    GetTypeArgv(&protocolFlag, argv);

    int fd = Socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));

    uint8_t *dataBuffer = (uint8_t *) malloc(BUF_SIZE);
    IsNull(dataBuffer, "dataBuffer is null");
    while (1) {
        Recv(fd, dataBuffer, BUF_SIZE, 0);
        struct iphdr *ipHeader = (struct iphdr *) (dataBuffer + sizeof(struct ethhdr));
        uint8_t protocol = ipHeader->protocol;

        if (protocolFlag == -1 || protocol == protocolFlag) {
            ProcessPacket(dataBuffer);
        }
    }
//    close(fd);
//    free(dataBuffer);
//
//    return 0;
}