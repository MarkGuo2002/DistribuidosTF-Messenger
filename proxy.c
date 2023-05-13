#include "list_utils.h"
#include "server.h"
#include "lines.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>



int main(){
    init();
    char clntUsername[256];
    char clntAlias[256];
    char clntBirthdate[16];
    
    strcpy(clntUsername, "Mark");
    strcpy(clntAlias, "MoyingPlays");
    strcpy(clntBirthdate, "15/10/2002");
    clntRegister(clntUsername, clntAlias, clntBirthdate);
    printList();



    strcpy(clntUsername, "Juan");
    strcpy(clntAlias, "Juanito");
    strcpy(clntBirthdate, "27/11/2002");
    clntRegister(clntUsername, clntAlias, clntBirthdate);
    

    strcpy(clntUsername, "Carlos");
    strcpy(clntAlias, "Carlitos");
    strcpy(clntBirthdate, "27/06/1993");
    clntRegister(clntUsername, clntAlias, clntBirthdate);
    printList();


    strcpy(clntUsername, "Juan");
    unregister(clntUsername);
    printList();

    unregister("Hello");
    
    return 0;
}