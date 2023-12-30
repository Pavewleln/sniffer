#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/ethernet.h>
#include "include/utils.h"
#include "include/erproc.h"
#include "include/transportLayer.h"
#include "interface/include/windowInit.h"

#define BUF_SIZE 65536

struct ThreadData {
    int fd;
    struct GtkWidgets *widgets;
};

static void ProcessPacket(const uint8_t *dataBuffer, const ssize_t dataLength, struct GtkWidgets *widgets) {
    // IP info
    struct iphdr *ipHeader = (struct iphdr *) (dataBuffer + sizeof(struct ethhdr));
    uint version = ipHeader->version;
    uint8_t protocol = ipHeader->protocol;
    uint8_t ttl = ipHeader->ttl;
    char sourceIp[INET_ADDRSTRLEN];
    char destinationIp[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ipHeader->saddr, sourceIp, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &ipHeader->daddr, destinationIp, INET_ADDRSTRLEN);

    char dateString[64];
    GetCurrentDate(dateString, sizeof(dateString));
    printf("Packet info (len: %ld): %s, IPv%d, ttl %d, ", dataLength, dateString, version, ttl);

    char text[256];
    snprintf(text, sizeof(text), "Packet info (len: %ld): %s, IPv%d, ttl %d, ", dataLength, dateString, version, ttl);

    // Создание метки и добавление в список
    GtkWidget *label = gtk_label_new(text);
    gtk_list_box_insert(GTK_LIST_BOX(widgets->listbox), label, -1);

    // Вывод данных в метку
    const gchar *labelText = gtk_label_get_text(GTK_LABEL(label));
    g_print("%s\n", labelText);

    switch (protocol) {
        case IPPROTO_TCP:
            PrintInfoTCP(sourceIp, destinationIp, ipHeader, dataBuffer, dataLength);
            break;
        case IPPROTO_UDP:
            PrintInfoUDP(sourceIp, destinationIp, ipHeader, dataBuffer, dataLength);
            break;
        case IPPROTO_ICMP:
            PrintInfoICMP(sourceIp, destinationIp, ipHeader, dataBuffer);
            break;
        default:
            break;
    }
}

void *PacketProcessingThread(void *data) {
    struct ThreadData *threadData = (struct ThreadData *) data;
    int fd = threadData->fd;
    struct GtkWidgets *widgets = threadData->widgets;
    uint8_t *dataBuffer = (uint8_t *) malloc(BUF_SIZE * sizeof(uint8_t));
    IsNull(dataBuffer, "Failed to allocate memory for data buffer");
    while (1) {
        ssize_t dataLength = Recv(fd, dataBuffer, BUF_SIZE, 0);
        if (dataLength <= (ssize_t)(sizeof(struct ethhdr) + sizeof(struct iphdr))) continue;
        ProcessPacket(dataBuffer, dataLength, widgets);
    }
    free(dataBuffer);
    return NULL;
}

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);
    struct GtkWidgets *widgets = WindowInit();

    int fd = Socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));

    // Создание и настройка данных для потока
    struct ThreadData threadData;
    threadData.fd = fd;
    threadData.widgets = widgets;

    // Создание потока для обработки пакетов
    pthread_t packetThread;
    pthread_create(&packetThread, NULL, PacketProcessingThread, (void *) &threadData);

    // Запуск главного цикла GTK
    gtk_main();

    // Ожидание завершения потока
    pthread_join(packetThread, NULL);

    return 0;
}