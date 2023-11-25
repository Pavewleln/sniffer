#ifndef ERPROC_H
#define ERPROC_H

int Socket(int domain, int type, int protocol);

void Recv(int sockfd, void *buf, size_t len, int flags);

#endif