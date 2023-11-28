#ifndef APPLICATION_LAYER_H
#define APPLICATION_LAYER_H

int IsHTTP(uint16_t sourcePort, uint16_t destinationPort);

int IsDNS(uint16_t sourcePort, uint16_t destinationPort);

int IsHTTPRequest(const uint8_t *tcpPayload, uint16_t tcpPayloadLen);

int IsHTTPResponse(const uint8_t *tcpPayload, uint16_t tcpPayloadLen);

void PrintInfoHTTP(uint16_t sourcePort, uint16_t destinationPort, struct tcphdr *tcpHeader, struct iphdr *ipHeader,
                   uint8_t *dataBuffer, uint16_t dataLength);

void PrintInfoDNS(uint16_t sourcePort, uint16_t destinationPort, struct iphdr *ipHeader, uint8_t *dataBuffer);

#endif