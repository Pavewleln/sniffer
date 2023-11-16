#ifndef MAIN_H
#define MAIN_H

void process_packet(char **argv);

void printInfoHTTP(uint16_t source_port, uint16_t destination_port, struct tcphdr *tcp_header);

void printInfoDNS(uint16_t source_port, uint16_t destination_port);

void printInfoTCP(const char *source_ip, const char *destination_ip);

void printInfoUDP(const char *source_ip, const char *destination_ip);

void printInfoICMP(const char *source_ip, const char *destination_ip);

#endif