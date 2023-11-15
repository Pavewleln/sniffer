#include <stdio.h>
#include <stdlib.h>

void Die(int result, const char *message) {
    if (result == -1) {
        perror(message);
        exit(EXIT_FAILURE);
    }
}