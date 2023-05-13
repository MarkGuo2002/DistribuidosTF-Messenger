// Declaration of variables and functions for a singly linked list
#ifndef SERVER_H
#define SERVER_H

int init();
int clntRegister(char* username, char* alias, char* birthdate);
int unregister(char* username);
int connect(char *alias, char *ip, int port);
int disconnect(char *alias);
int sendMessage(char* senderAlias, char* receiverAlias, char* message);


#endif
