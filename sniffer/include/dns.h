#ifndef DNS_H
#define DNS_H

typedef struct {
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
} dnsHeader;

typedef struct {
    uint16_t qtype;
    uint16_t qclass;
} dnsQuestion;

typedef struct {
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t dataLength;
    // Add additional fields as needed for different record types
} dnsAnswer;

typedef struct {
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t dataLength;
    // Add additional fields as needed for different record types
} dnsAuthority;

typedef struct {
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t dataLength;
    // Add additional fields as needed for different record types
} dnsAdditional;
#endif