#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "include/http_output.h"
#include "include/utils.h"

void printHttpRequest(const uint8_t *http_data, uint http_data_len) {
    printf("Received HTTP Request:\n");
    HttpRequest *httpHeader = (HttpRequest *) http_data;

    printf("HTTP Header:\n");
    printf("Method: %s\n", httpHeader->method);
    printf("URI: %s\n", httpHeader->uri);
    printf("Host: %s\n", httpHeader->host);
    printf("Content-Type: %s\n", httpHeader->content_type);
    printf("Content-Length: %s\n", httpHeader->content_length);
    dump(http_data + sizeof(HttpRequest), http_data_len);
}

void printHttpResponse(const uint8_t *http_data, uint http_data_len) {
    printf("Received HTTP Response:\n");
    HttpResponse *httpHeader = (HttpResponse *) http_data;

    printf("HTTP Header:\n");
    printf("Method: %d\n", httpHeader->status_code);
    printf("URI: %s\n", httpHeader->status_text);
    printf("Host: %s\n", httpHeader->content_length);
    printf("Host: %s\n", httpHeader->content_type);
    dump(http_data + sizeof(HttpResponse), http_data_len);
}