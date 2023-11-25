#ifndef UTILS_H
#define UTILS_H

void GetCurrentDate(char *date_string, size_t max_length);

void GetTypeArgv(int *protocolFlag, char** argv);

void Dump(const unsigned char *data, uint data_len);

void IsError(int result, const char *errorMessage);

void IsNull(void *result, const char *errorMessage);

#endif