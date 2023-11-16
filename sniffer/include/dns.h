#ifndef DNS_H
#define DNS_H

typedef struct {
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
} dns_header;

typedef struct {
    uint16_t qtype;
    uint16_t qclass;
} dns_question;

typedef struct {
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t data_length;
    // Add additional fields as needed for different record types
} dns_answer;

typedef struct {
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t data_length;
    // Add additional fields as needed for different record types
} dns_authority;

typedef struct {
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t data_length;
    // Add additional fields as needed for different record types
} dns_additional;
#endif