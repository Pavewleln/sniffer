#ifndef APPLICATION_LAYER_H
#define APPLICATION_LAYER_H

void PrintInfoHTTP(uint16_t sourcePort, uint16_t destinationPort, struct tcphdr *tcp_header, struct iphdr* ipHeader, uint8_t * dataBuffer, uint16_t totalLen);

void PrintInfoDNS(uint16_t sourcePort, uint16_t destinationPort, struct iphdr* ipHeader, uint8_t * dataBuffer);

#endif