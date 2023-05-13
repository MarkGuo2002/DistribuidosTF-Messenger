// Declaration of variables and functions for a singly linked list
#ifndef SERVER_H
#define SERVER_H

int init();
int clntRegister(char* username, char* alias, char* birthdate);
int clntUnregister(char* username);
int clntConnect(char *alias, char *ip, int port);
int clntDisconnect(char *alias);
int clntSendMessage(char* senderAlias, char* receiverAlias, char* message);


#endif
