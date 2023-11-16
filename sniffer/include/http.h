#ifndef HTTP_H
#define HTTP_H
// Структура для представления HTTP-запроса
typedef struct {
    int8_t method[16];          // Метод запроса (например, "GET", "POST", "PUT")
    int8_t uri[256];            // URI ресурса
    int8_t host[256];           // Значение заголовка "Host"
    int8_t content_type[64];    // Значение заголовка "Content-Type"
    int8_t content_length[16];  // Значение заголовка "Content-Length"
} HttpRequest;

// Структура для представления HTTP-ответа
typedef struct {
    int status_code;          // Код состояния ответа (например, 200, 404, 500)
    int8_t status_text[64];     // Текстовое описание кода состояния
    int8_t content_type[64];    // Значение заголовка "Content-Type"
    int8_t content_length[16];  // Значение заголовка "Content-Length"
} HttpResponse;
#endif