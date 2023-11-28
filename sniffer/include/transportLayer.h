#ifndef TRANSPORT_LAYER_H
#define TRANSPORT_LAYER_H

void PrintInfoTCP(const char *sourceIp, const char *destinationIp, struct iphdr* ipHeader, uint8_t * dataBuffer, uint16_t dataLength);

void PrintInfoUDP(const char *sourceIp, const char *destinationIp, struct iphdr* ipHeader, uint8_t * dataBuffer);

void PrintInfoICMP(const char *sourceIp, const char *destinationIp, struct iphdr* ipHeader, uint8_t * dataBuffer);

#endif