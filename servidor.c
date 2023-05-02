/*Implementation of a singly linked clntList*/
#include "servidor.h"
#include "list.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>




/*
void deleteList(){
    struct ClientNode *aux = clntList->head;
    struct ClientNode *prev = NULL;
    while (aux != NULL){
        prev = aux;
        aux = aux->next;
        free(prev);
    }
    free(clntList);
}
*/

int init(){
     //writer
    // Queda por implementar si la lista ya existe y tiene que borrar
    clntList = (struct clientList *) malloc(sizeof(struct clientList));
    clntList->head = NULL;
    clntList->tail = NULL;
    clntList->size = 0;
	return 0;
}

int clntRegister(char* username){
    //first check if the username is already in the list
    struct ClientNode *aux = clntList->head;
    while (aux != NULL){
        if (strcmp(aux->username, username) == 0){
            printf("Username already in use\n");
            return -1;
        }
        aux = aux->next;
    }
    //if the username is not in the list, create a new node and add it to the list
    struct ClientNode *newNode = (struct ClientNode *) malloc(sizeof(struct ClientNode));
    //allocate memory space for the username
    newNode->username = (char *) malloc(strlen(username) + 1);
    strcpy(newNode->username, username);
    newNode->alias = NULL;
    newNode->birthdate = NULL;
    newNode->status = 0;
    newNode->ip = NULL;
    newNode->port = 0;
    newNode->pendingMsgList = NULL;
    newNode->lastMessageId = 0;
    newNode->next = NULL;

    if (clntList->head == NULL){
        clntList->head = newNode;
        clntList->tail = newNode;
    }
    else{
        clntList->tail->next = newNode;
        clntList->tail = newNode;
    }
    clntList->size++;
    return 0;

}


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


int main(){
    init();
    char clntUsername[256];
    strcpy(clntUsername, "Tu puta madre");
    clntRegister(clntUsername);
    printList();
    strcpy(clntUsername, "Me comes la polla");
    clntRegister(clntUsername);
    strcpy(clntUsername, "Subnormal");
    clntRegister(clntUsername);
    printf("Hello world\n");
    printList();
    return 0;
}