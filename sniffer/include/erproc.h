#ifndef ERPROC_H
#define ERPROC_H

void TestingSnifferFunctions();

int Socket(int domain, int type, int protocol);

int Recv(int sockfd, void *buf, size_t len, int flags);

#endif