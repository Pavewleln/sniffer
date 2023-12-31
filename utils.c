#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <netinet/in.h>
#include "include/utils.h"

void GetCurrentDate(char *dateString, const size_t dataLength) {
    time_t currentTime = time(NULL);
    struct tm *localTime = localtime(&currentTime);
    strftime(dateString, dataLength, "%Y-%m-%d, %H:%M:%S", localTime);
}

int GetTypeArgv(char **argv, int argc) {
    int protocolFlag = -1;
    if (argc < 2) {
        printf("Usage: sudo ./argv[0] <protocol>(TCP/UDP/ICMP/ALL)\n");
        exit(EXIT_FAILURE);
    }
    if (strcmp(argv[1], "TCP") == 0) {
        protocolFlag = IPPROTO_TCP;
    } else if (strcmp(argv[1], "UDP") == 0) {
        protocolFlag = IPPROTO_UDP;
    } else if (strcmp(argv[1], "ICMP") == 0) {
        protocolFlag = IPPROTO_ICMP;
    } else if (strcmp(argv[1], "ALL") != 0) {
        printf("Flag not found\n");
        exit(EXIT_FAILURE);
    }
    return protocolFlag;
}

void Dump(const uint8_t *data, const uint dataLength) {
    for (uint i = 0; i < dataLength; i++) {
        uint byte = data[i];
        if ((i % 16) == 0) {
            printf("%04x  ", i);
        }
        printf("%02x ", byte);
        if (((i % 16) == 15) || (i == dataLength - 1)) {
            for (uint j = 0; j < 15 - (i % 16); j++) {
                printf(" ");
            }
            printf("| ");
            for (uint j = i - (i % 16); j <= i; j++) {
                byte = data[j];
                if ((byte > 31) && byte < 127) {
                    printf("%c", byte);
                } else {
                    printf(".");
                }
            }
            printf("\n");
        }
    }
}

void IsError(int result, char *errorMessage) {
    if (result == -1) {
        perror(errorMessage);
        exit(EXIT_FAILURE);
    }
}

void IsNull(void *result, char *errorMessage) {
    if (result == NULL) {
        perror(errorMessage);
        exit(EXIT_FAILURE);
    }
}
