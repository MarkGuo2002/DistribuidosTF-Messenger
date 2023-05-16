
#include "list_utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>



char * strMalloc(char *str){
    // this function allocates the memory for the input string and returns a pointer to the new string
    char *newStr = (char *) malloc(strlen(str) + 1);
    if (newStr == NULL){
        printf("Error allocating memory for string\n");
        return NULL;
    }
    strcpy(newStr, str);
    return newStr;
}

//these are client list utils functions

void printList(){
    struct ClientNode *aux = clntList->head;
    if (aux == NULL){
        printf("clntList is empty\n");
        return;
    }
    while (aux != NULL){
        //split the print in two lines for better readability  
        printf("Username: %s\nAlias: %s\nBirthdate: %s\nStatus: %d\nIP: %s\nPort: %d\n"
        "Last msg ID: %u\n\n", aux->username, aux->alias, aux->birthdate, aux->status, aux->ip, aux->port, aux->lastMessageId);

        aux = aux->next;
    }
    printf("\n\n");
}

//returns client node by username
struct ClientNode *findUsername(char* username){
    if (clntList->size == 0){
        printf("clntList is empty\n");
        return NULL;
    }
    struct ClientNode *aux = clntList->head;
    while (aux != NULL){
        if (strcmp(aux->username, username) == 0){
            return aux;
        }
        aux = aux->next;
    }
    return NULL;
}

//returns client node by alias
struct ClientNode *findAlias(char* alias){
    if (clntList->size == 0){
        printf("clntList is empty\n");
        return NULL;
    }
    struct ClientNode *aux = clntList->head;
    while (aux != NULL){
        if (strcmp(aux->alias, alias) == 0){
            return aux;
        }
        aux = aux->next;
    }
    return NULL;
}

//these are pending message list utils functions

int appendMsgNode(struct PendingMessageList* ClntPendingMsgList, int id, char* message, char* aliasSender, char* aliasReceiver){
    struct PendingMessageNode *newMsgNode = (struct PendingMessageNode *) malloc(sizeof(struct PendingMessageNode));
    newMsgNode->id = id;
    newMsgNode->message = strMalloc(message);
    newMsgNode->aliasSender = strMalloc(aliasSender);
    newMsgNode->aliasReceiver = strMalloc(aliasReceiver); 
    
    
    //make a tail insertion
    if (ClntPendingMsgList->head == NULL){
        ClntPendingMsgList->head = newMsgNode;
        ClntPendingMsgList->tail = newMsgNode;
        newMsgNode->next = NULL;
    }
    else{
        ClntPendingMsgList->tail->next = newMsgNode;
        ClntPendingMsgList->tail = newMsgNode;
        newMsgNode->next = NULL;
    }
    ClntPendingMsgList->size++;
    return 0;


}


struct PendingMessageNode* popHeadMessage(struct PendingMessageList* ClntPendingMsgList){
    if (ClntPendingMsgList->size == 0){
        printf("ClntPendingMsgList is empty\n");
        return NULL;
    }
    struct PendingMessageNode *aux = ClntPendingMsgList->head;
    ClntPendingMsgList->head = ClntPendingMsgList->head->next;
    ClntPendingMsgList->size--;
    return aux;
}

void printMessageList(struct PendingMessageList* ClntPendingMsgList){
    struct PendingMessageNode *aux = ClntPendingMsgList->head;
    if (aux == NULL){
        printf("ClntPendingMsgList is empty\n");
        return;
    }
    while (aux != NULL){
        //split the print in two lines for better readability  
        printf("ID: %d\nMessage: %s\nSender: %s\nReceiver: %s\n\n", aux->id, aux->message, aux->aliasSender, aux->aliasReceiver);
        aux = aux->next;
    }
    printf("\n\n");
}