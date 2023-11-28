#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <netinet/in.h>
#include "include/utils.h"

void GetCurrentDate(char *dateString, size_t maxLength) {
    time_t currentTime = time(NULL);
    struct tm *localTime = localtime(&currentTime);
    strftime(dateString, maxLength, "%Y-%m-%d", localTime);
}

void GetTypeArgv(int *protocolFlag, char **argv, int argc) {
    if (argc < 2) {
        printf("Usage: sudo ./program <protocol>(TCP/UDP/ICMP/ALL)\n");
        return;
    }
    if (strcmp(argv[1], "TCP") == 0) {
        *protocolFlag = IPPROTO_TCP;
    } else if (strcmp(argv[1], "UDP") == 0) {
        *protocolFlag = IPPROTO_UDP;
    } else if (strcmp(argv[1], "ICMP") == 0) {
        *protocolFlag = IPPROTO_ICMP;
    } else if (strcmp(argv[1], "ALL") != 0) {
        printf("Flag not found\n");
        return;
    }
}

void Dump(const uint8_t *data, uint data_len) {
    for (uint i = 0; i < data_len; i++) {
        printf("%c", data[i]);
    }
    printf("\n");
}

void IsError(int result, const char *errorMessage) {
    if (result == -1) {
        perror(errorMessage);
        exit(EXIT_FAILURE);
    }
}

void IsNull(void *result, const char *errorMessage) {
    if (result == NULL) {
        perror(errorMessage);
        exit(EXIT_FAILURE);
    }
}
