#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/ethernet.h>
#include "include/utils.h"
#include "include/erproc.h"
#include "include/transportLayer.h"

#define BUF_SIZE 65536

static void
ProcessPacket(uint8_t *dataBuffer, ssize_t dataLength) {
    // IP info
    struct iphdr *ipHeader = (struct iphdr *) (dataBuffer + sizeof(struct ethhdr));
    uint version = ipHeader->version;
    uint8_t protocol = ipHeader->protocol;
    uint8_t ttl = ipHeader->ttl;
    char sourceIp[INET_ADDRSTRLEN];
    char destinationIp[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ipHeader->saddr, sourceIp, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &ipHeader->daddr, destinationIp, INET_ADDRSTRLEN);

    char dateString[64];
    GetCurrentDate(dateString, sizeof(dateString));
    printf("Packet info (len: %ld): %s, IPv%d, ttl %d, ", dataLength, dateString, version, ttl);

    switch (protocol) {
        case IPPROTO_TCP:
            PrintInfoTCP(sourceIp, destinationIp, ipHeader, dataBuffer, dataLength);
            break;
        case IPPROTO_UDP:
            PrintInfoUDP(sourceIp, destinationIp, ipHeader, dataBuffer, dataLength);
            break;
        case IPPROTO_ICMP:
            PrintInfoICMP(sourceIp, destinationIp, ipHeader, dataBuffer);
            break;
        default:
            printf("Unknown Protocol\n");
            break;
    }
}

int
main(int argc, char **argv) {
    int protocolFlag = -1;
    GetTypeArgv(&protocolFlag, argv, argc);

    int fd = Socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));

    uint8_t dataBuffer[BUF_SIZE];
    IsNull(dataBuffer, "Failed to allocate memory for data buffer");
    while (1) {
        ssize_t dataLength = Recv(fd, dataBuffer, BUF_SIZE, 0);
        if (dataLength <= (ssize_t)(sizeof(struct ethhdr) + sizeof(struct iphdr))) continue;
        struct iphdr *ipHeader = (struct iphdr *) (dataBuffer + sizeof(struct ethhdr));
        uint8_t protocol = ipHeader->protocol;

        if (protocolFlag == -1 || protocol == protocolFlag) ProcessPacket(dataBuffer, dataLength);
    }
}
