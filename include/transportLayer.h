#ifndef TRANSPORT_LAYER_H
#define TRANSPORT_LAYER_H

void PrintInfoTCP(const char *sourceIp, const char *destinationIp, const struct iphdr *ipHeader, const uint8_t *dataBuffer,
             const uint dataLength);

void PrintInfoUDP(const char *sourceIp, const char *destinationIp, const struct iphdr *ipHeader, const uint8_t *dataBuffer,
             const uint dataLength);

void PrintInfoICMP(const char *sourceIp, const char *destinationIp, const struct iphdr *ipHeader, const uint8_t *dataBuffer);

#endif