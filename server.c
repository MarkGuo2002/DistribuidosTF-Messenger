/*Implementation of a singly linked clntList*/
#include "list_utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


struct clientList *clntList;


//server functions 

int init(){
     //writer
    // Queda por implementar si la lista ya existe y tiene que borrar
    clntList = (struct clientList *) malloc(sizeof(struct clientList));
    clntList->head = NULL;
    clntList->tail = NULL;
    clntList->size = 0;
	return 0;
}

int clntRegister(char* username, char* alias, char* birthdate){
    //first check if the username is already in the list
    struct ClientNode *aux = clntList->head;
    while (aux != NULL){
        if (strcmp(aux->username, username) == 0){
            printf("Username already in use\n");
            return 1;
        }
        aux = aux->next;
    }
    //if the username is not in the list, create a new node and add it to the list
    struct ClientNode *newNode = (struct ClientNode *) malloc(sizeof(struct ClientNode));
    //allocate memory space for the username

    newNode->username = strMalloc(username);
    newNode->alias = strMalloc(alias);
    newNode->birthdate = strMalloc(birthdate);
    newNode->status = 0;
    newNode->ip = NULL;
    newNode->port = 0;
    newNode->pendingMsgList = (struct PendingMessageList *) malloc(sizeof(struct PendingMessageList));
    newNode->lastMessageId = 0;
    

    if (clntList->head == NULL){
        clntList->head = newNode;
        clntList->tail = newNode;
    }
    else{

        clntList->tail->next = newNode;
        newNode->prev = clntList->tail;
        newNode->next = NULL;
        clntList->tail = newNode;
    }
    clntList->size++;
    return 0;

}


int clntUnregister(char* username){
    struct ClientNode *aux = findUsername(username);
    if (aux == NULL){
        printf("Username not found\n");
        return 1;
    }
    if (aux->prev == NULL){
        clntList->head = aux->next;
    }
    else{
        aux->prev->next = aux->next;
    }
    if (aux->next == NULL){
        clntList->tail = aux->prev;
    }
    else{
        aux->next->prev = aux->prev;
    }
    clntList->size--;
    free(aux);
    return 0;
}


int clntConnect(char *alias, char *ip, int port){
    struct ClientNode *aux = findAlias(alias);
    if (aux == NULL){
        printf("Alias not found\n");
        return 1;
    }
    if (aux->status == 1){
        printf("Client already connected\n");
        return 2;
    }
    else{
        //need to send all the pending messages to the just connect user
        aux->ip = strMalloc(ip);
        aux->port = port;
        aux->status = 1;
        // here should check if the pending message list is empty, if not, send all the messages to socket


        return 0;
    }
    return 3;
}

int clntDisconnect(char *alias){
    struct ClientNode *aux = findAlias(alias);
    if (aux == NULL){
        printf("Alias not found\n");
        return 1;
    }
    if (aux->status == 0){
        printf("Client already disconnected\n");
        return 2;
    }
    else{
        aux->ip = NULL;
        aux->port = 0;
        aux->status = 0;
        return 0;
    }
    return 3;
}

int clntSendMessage(char* senderAlias, char* receiverAlias, char* message){
    struct ClientNode * sendernode = findAlias(senderAlias);
    struct ClientNode * receivernode = findAlias(receiverAlias);
    if ((sendernode == NULL) || (receivernode == NULL)){
        printf("Sender alias not found\n");
        return 1;
    }
    //now call this function int appendMsgNode(struct PendingMessageList* ClntPendingMsgList, int id, char* message, char* aliasSender, char* aliasReceiver)
    appendMsgNode(receivernode->pendingMsgList, sendernode->lastMessageId, message, senderAlias, receiverAlias);
    sendernode->lastMessageId++;
    return 0;

    
    

}

//int printAllUserAlias(){}


