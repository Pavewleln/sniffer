#ifndef APPLICATION_LAYER_H
#define APPLICATION_LAYER_H
void PrintInfoHTTP(const struct iphdr *ipHeader, const struct tcphdr *tcpHeader, const uint8_t *dataBuffer, const uint dataLength);

void PrintInfoDNS(const struct iphdr *ipHeader, const uint8_t *dataBuffer, const uint dataLength, const size_t transportHdr);

#endif