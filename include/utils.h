#ifndef UTILS_H
#define UTILS_H

void GetCurrentDate(char *date_string, const size_t dataLength);

int GetTypeArgv(char **argv, int argc);

void Dump(const unsigned char *data, const uint dataLength);

void IsError(int result, char *errorMessage);

void IsNull(void *result, char *errorMessage);

#endif