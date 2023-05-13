#include <unistd.h>

int socketSendMessage(int socket, char *buffer, int len);
int socketRecvMessage(int socket, char *buffer, int len);
ssize_t socketReadLine(int fd, void *buffer, size_t n);
