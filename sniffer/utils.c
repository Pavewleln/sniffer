#include <stdio.h>
#include <stdint.h>
#include <netinet/udp.h>
#include "include/utils.h"

void dump(const uint8_t *data, uint data_len) {
    for (uint i = 0; i < data_len; i++) {
        printf("%c", data[i]);
    }
    printf("\n");
}