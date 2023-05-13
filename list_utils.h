#ifndef LIST_UTILS_H
#define LIST_UTILS_H

#include "list.h"


char * strMalloc(char *str);
void printList();
struct ClientNode *findUsername(char* username);
struct ClientNode *findAlias(char* alias);
int appendMsgNode(struct PendingMessageList* ClntPendingMsgList, int id, char* message, char* aliasSender, char* aliasReceiver);

#endif
