#ifndef APPLICATION_LAYER_H
#define APPLICATION_LAYER_H

void PrintInfoHTTP(struct iphdr *ipHeader, struct tcphdr *tcpHeader, uint8_t *dataBuffer, const uint dataLength);

void PrintInfoDNS(struct iphdr *ipHeader, uint8_t *dataBuffer, const uint dataLength, size_t transportHdr);

#endif