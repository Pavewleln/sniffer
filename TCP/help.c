#include <stdio.h>
#include <stdlib.h>
void Die (char *c) {
    perror(c);
    exit(EXIT_FAILURE);
}