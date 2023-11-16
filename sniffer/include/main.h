#ifndef CONFIG_H
#define CONFIG_H

void process_packet(char **argv);

void dump(const uint8_t *data_buffer, uint data_len);

void printHttpRequest(const uint8_t *http_data, uint http_data_len);

void printHttpResponse(const uint8_t *http_data, uint http_data_len);

void printInfoHTTP(uint16_t source_port, uint16_t destination_port, struct tcphdr *tcp_header);

void printInfoDNS(uint16_t source_port, uint16_t destination_port);

void printInfoTCP(const char *source_ip, const char *destination_ip);

void printInfoUDP(const char *source_ip, const char *destination_ip);

void printInfoICMP(const char *source_ip, const char *destination_ip);

#endif