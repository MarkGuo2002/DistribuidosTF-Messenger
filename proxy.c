#include "list_utils.h"
#include "server.h"
#include "lines.h"
#include "list.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define MAX 256

int port;
int sd;
int newsd;
socklen_t usersize;
int val;
int err;
// sockadd_in is a structure that contains the address of the socket of type IPv4
struct sockaddr_in server_addr; 
struct sockaddr_in client_addr;


void test(){
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
    clntUnregister(clntUsername);
    printList();

    clntUnregister("Hello");
    
    struct ClientNode *aux = findUsername("Mark");
    appendMsgNode(aux->pendingMsgList, 1, "Hello", "MoyingPlays", "Juanito");
    appendMsgNode(aux->pendingMsgList, 2, "Hello", "Masasdasdasadasdsasad", "Juanito");
    appendMsgNode(aux->pendingMsgList, 3, "Hello", "MoyingPlays", "ddddddddo");
    printMessageList(aux->pendingMsgList);

    struct PendingMessageNode *messageNode;
    while (aux->pendingMsgList->size > 0){
            //popHeadMessage will return the message and remove it from the list
            messageNode = popHeadMessage(aux->pendingMsgList);
            printf("Message ID: %d\n", messageNode->id);
            printMessageList(aux->pendingMsgList);
    }

}

int getSocketfromIpPort(char* ip, int port){
    int client_listen_sd;
    struct sockaddr_in client_listen_addr;
    struct hostent *hp;
    client_listen_sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_listen_sd < 0){
        perror("socket() failed");
        exit(-1);
    }
    hp = gethostbyname(ip);
    if (hp == NULL){
        perror("gethostbyname() failed");
        exit(-1);
    }
    bzero((char *)&client_listen_addr, sizeof(client_listen_addr));
    memcpy(&(client_listen_addr.sin_addr), hp->h_addr, hp->h_length);
    client_listen_addr.sin_family = AF_INET;
    client_listen_addr.sin_port = htons(port);
    //connect to the server+
    if (connect(client_listen_sd, (struct sockaddr *)&client_listen_addr, sizeof(client_listen_addr)) < 0){
        perror("connect() failed");
        exit(-1);
    }
    return client_listen_sd;
}


int sendMessageInList(struct ClientNode * clnt){
    /*In this function, first gets the receiver's listening thread's socket by it ip and port.
    Then, it sends the operation, aliasSender, id and to the receiver's listening thread.
    */
    
    struct PendingMessageNode * messageNode;
    int receiver_listen_sd = getSocketfromIpPort(clnt->ip, clnt->port);
    char operation[MAX];
    strcpy(operation, "SEND_MESSAGE");
    while (clnt->pendingMsgList->size > 0){
        //popHeadMessage will return the message and remove it from the list
        messageNode = popHeadMessage(clnt->pendingMsgList);
        printf("SEND MESSAGE %d FROM %s TO %s\n", messageNode->id, messageNode->aliasSender, messageNode->aliasReceiver);
        //send the message to the client
        if (socketSendMessage(receiver_listen_sd, operation, strlen(operation)+1) < 0) {
            perror("Error in send");
            return 1;
        }
        printf("operation sent\n");
        if (socketSendMessage(receiver_listen_sd, messageNode->aliasSender, strlen(messageNode->aliasSender)+1) < 0) {
            perror("Error in send");
            return 1;
        }
        printf("aliasSender sent\n");
        //send the messageNode->id as well
        char id[MAX];
        sprintf(id, "%d", messageNode->id);
        if (socketSendMessage(receiver_listen_sd, id, strlen(id)+1) < 0) {
            perror("Error in send");
            return 1;
        }
        printf("id sent\n");
        if (socketSendMessage(receiver_listen_sd, messageNode->message, strlen(messageNode->message)+1) < 0) {
            perror("Error in send");
            return 1;
        }
        printf("message sent\n");

    }
    return 0;
}



void treatRequest(int newsd){ 
    char buf[MAX];
    char reply[MAX];
    ssize_t bytes_read;

    if ((bytes_read = socketReadLine(newsd, buf, MAX)) > 0) {
        //buf[bytes_read] = '\0';
        printf("Received: %s\n", buf);
        printf("bytes_read: %ld\n", bytes_read);
    }
    //entering all the cases
    if (strcmp(buf, "REGISTER")==0){
        printf("Treating REGISTER\n");
        char username[MAX];
        char alias[MAX];
        char birthdate[16];
        

        //read socket and store in username, alias and birthdate
        if ((bytes_read = socketReadLine(newsd, username, MAX)) > 0) {
            printf("Received: %s\n", username);
            printf("bytes_read: %ld\n", bytes_read);
        }
        if ((bytes_read = socketReadLine(newsd, alias, MAX)) > 0) {
            printf("Received: %s\n", alias);
            printf("bytes_read: %ld\n", bytes_read);
        }
        if ((bytes_read = socketReadLine(newsd, birthdate, MAX)) > 0) {
            printf("Received: %s\n", birthdate);
            printf("bytes_read: %ld\n", bytes_read);
        }
        //register the client
        int res = clntRegister(username, alias, birthdate);
        if(res == 1){
            strcpy(reply, "1");
        }
        else if(res == 0){
            strcpy(reply, "0");
        }
        else{
            strcpy(reply, "2");
        }
        //send back the reply
        if (socketSendMessage(newsd, reply, strlen(reply) + 1) < 0) {
            perror("Error in send");
            exit(1);
        }
        if (socketSendMessage(newsd, reply, sizeof(char)) < 0) {
        perror("Error in send");
        exit(1);
    }

    } else if (strcmp(buf, "UNREGISTER")==0) {
        printf("Treating UNREGISTER\n");
        char alias[MAX];
        

        //read socket and store in usernam
        if ((bytes_read = socketReadLine(newsd, alias, MAX)) > 0) {
            printf("Received alias: %s\n", alias);
            printf("bytes_read: %ld\n", bytes_read);
        }
        //unregister the client
        int res = clntUnregister(alias);
        if(res == 1){
            strcpy(reply, "1");
        }
        else if(res == 0){
            strcpy(reply, "0");
        }
        else{
            strcpy(reply, "2");
        }
        if (socketSendMessage(newsd, reply, sizeof(char)) < 0) {
        perror("Error in send");
        exit(1);
    }

    } else if (strcmp(buf, "CONNECT")==0) {
        printf("Treating CONNECT\n");
        char alias[MAX];
        char clnt_port[MAX];


        //read socket and store in usernam
        if ((bytes_read = socketReadLine(newsd, alias, MAX)) > 0) {
            printf("Received alias: %s\n", alias);
            printf("bytes_read: %ld\n", bytes_read);
        }
        if ((bytes_read = socketReadLine(newsd, clnt_port, MAX)) > 0) {
            printf("Received port: %s\n", clnt_port);
            printf("bytes_read: %ld\n", bytes_read);
        }
        //obtain client ip address from client_addr
        char clnt_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), clnt_ip, INET_ADDRSTRLEN);
        //cast clnt_port to int
        int clnt_port_int = atoi(clnt_port);
        //connect the client
        int res = clntConnect(alias, clnt_ip, clnt_port_int);
        if(res == 1){
            //alias not found
            strcpy(reply, "1");
        }
        else if(res == 0){
            // success
            strcpy(reply, "0");
        }
        else if (res == 2){
            //client already connected
            strcpy(reply, "2");
        }
        else{
            strcpy(reply, "3");
        }

        //send back the reply
        if (socketSendMessage(newsd, reply, sizeof(char)) < 0) {
            perror("Error in send");
            exit(1);
        }
        struct ClientNode* clntNode = findAlias(alias);
        if (clntNode->pendingMsgList->size > 0){
            printf("YES THERE ARE PENDING MESSAGES\n");
            //send the pending messages
            sendMessageInList(clntNode);
        }
       


    } else if (strcmp(buf, "DISCONNECT")==0) {
        printf("Treating DISCONNECT\n");
        char alias[MAX];

        //read socket and store in usernam
        if ((bytes_read = socketReadLine(newsd, alias, MAX)) > 0) {
            printf("Received alias: %s\n", alias);
            printf("bytes_read: %ld\n", bytes_read);
        }
        
        //disconnect the client
        int res = clntDisconnect(alias);
        if(res == 1){
            //alias not found
            strcpy(reply, "1");
        }
        else if(res == 0){
            // success
            strcpy(reply, "0");
        }
        else if (res == 2){
            //client not connected
            strcpy(reply, "2");
        }
        else{
            strcpy(reply, "3");
        }
        if (socketSendMessage(newsd, reply, sizeof(char)) < 0) {
            perror("Error in send");
            exit(1);
        }

    } else if (strcmp(buf, "SEND")==0) {
        /*First we read aliases and message from client through socket
        After that clntSendMessage does the first half of the process, which checks if both
        users exist and if so, the message with its corresponding fields filled will be appended
        at the end of the PendingMessageList.
    
        after that it sends back the result of the first half, if the first half was successful(0),
        the program checks if the receiver is online, if it is, send his pending messages to him */
        printf("Treating SEND\n");
        char reply[MAX];
        char aliasSender[MAX];
        char aliasReceiver[MAX];
        char message[MAX];
        if ((bytes_read = socketReadLine(newsd, aliasSender, MAX)) > 0) {
            printf("Received aliasSender: %s\n", aliasSender);
            printf("bytes_read: %ld\n", bytes_read);
        }
        if ((bytes_read = socketReadLine(newsd, aliasReceiver, MAX)) > 0) {
            printf("Received aliasReceiver: %s\n", aliasReceiver);
            printf("bytes_read: %ld\n", bytes_read);
        }
        if ((bytes_read = socketReadLine(newsd, message, MAX)) > 0) {
            printf("Received message: %s\n", message);
            printf("bytes_read: %ld\n", bytes_read);
        }
        // send the message, only appends to the list if everything correct, nothing else
        int res = clntSendMessage(aliasSender, aliasReceiver, message);
        sprintf(reply, "%d", res);
        if (socketSendMessage(newsd, reply, sizeof(char)) < 0) {
            perror("Error in send");
            exit(1);
        }
        if (res == 0){
            printf("Message stored in list\n");

            struct ClientNode * sendernode = findAlias(aliasSender);
            struct ClientNode * receivernode = findAlias(aliasReceiver);
            int id = sendernode->lastMessageId - 1;
            char idstr[10];
            sprintf(idstr, "%d", id);
            strcpy(reply, idstr);
            if (socketSendMessage(newsd, reply, strlen(reply) + 1) < 0) {
                perror("Error in send");
                exit(1);
            }
            //now check if the receiver is connected, if it is, send the message
            if (receivernode->status == 1){
                sendMessageInList(receivernode);
                //the printing is done in the sendMessageInList function

            }
            else{
                printf("MESSAGE %d FROM %s TO %s STORED", id, aliasSender, aliasReceiver);

            }
        
        
        
        }else if (res == 1){
            printf("Alias not found\n");
            strcpy(reply, "1");
            if (socketSendMessage(newsd, reply, sizeof(char)) < 0) {
                perror("Error in send");
            exit(1);
            }
        }else{
            strcpy(reply, "2");
            if (socketSendMessage(newsd, reply, sizeof(char)) < 0) {
                perror("Error in send");
            exit(1);
            }
        }
        
        
    }else if (strcmp(buf, "CONNECTEDUSERS")==0) {
        printf("Treating connectedusers\n");
        char alias[MAX];


        //read socket and store in usernam
        if ((bytes_read = socketReadLine(newsd, alias, MAX)) > 0) {
            printf("Received alias: %s\n", alias);
            printf("bytes_read: %ld\n", bytes_read);
        }
        //unregister the client
        int res = clntUnregister(alias);
        if(res == 1){
            strcpy(reply, "1");
        }
        else if(res == 0){
            strcpy(reply, "0");
        }
        else{
            strcpy(reply, "2");
        }
        if (socketSendMessage(newsd, reply, sizeof(char)) < 0) {
        perror("Error in send");
        exit(1);
        }
    }
    else{
        printf("Invalid command\n");
        strcpy(reply, "server said ERROR to your registration");
        if (socketSendMessage(newsd, reply, sizeof(char)) < 0) {
        perror("Error in send");
        exit(1);
    }
    }
    
    
    printf("\nList of clients:\n\n");
    printList();
    close(newsd);
    printf("Connection closed\n===================================================\n\n");
}






int main(int argc , char *argv[]){
    //test();
    //return 0;

    if (argc != 3) {
        printf("Usage: ./server -p [port_number]\n");
        return 1;
    }

    // Check if the argument is "-p"
    if (strcmp(argv[1], "-p") != 0) {
        printf("Invalid argument: %s\n", argv[1]);
        printf("Usage: ./server -p [portNumber]\n");
        return 1;
    }

    // Parse the port number
    int port = atoi(argv[2]);

    // Create server socket, bind, listen, etc. (your server logic here)
    sd =  socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sd < 0) {
            perror("Error in socket");
            exit(1);
    }

    val = 1;
    // setsockopt() is used to allow the local address to be reused when the server is restarted before the required wait time expires
    err = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));
    if (err < 0) {
            perror("Error in option");
            exit(1);
    }

  
    //2.-bind the socket to the server address, which means that the server will listen to the port 4200, a port is a process that handles specific port
    //bzero() is used to set the server_addr to 0 in order to avoid errors
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    //server_addr.sin_addr.s_addr  means that the server will listen to any IP address that is connected to the server
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
            perror("Error in bind");
            exit(1);
    }

    //3.-listen to the socket, SOMAXCONN is the maximum number of connections that can be queued, by default is 128.
    listen(sd, SOMAXCONN);



    // Get local IP address
    char localIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(server_addr.sin_addr), localIP, INET_ADDRSTRLEN);

    // Display local IP and port
    printf("Server is running on %s:%d\n", localIP, port);

    init();
    
    while (1){
        printf("Waiting for requests...\n");
    //4.- accept the connection, the server will wait until a client connects to the server, accept returns a new socket descriptor that is used to communicate with the client
        newsd=accept(sd, (struct sockaddr *)&client_addr, &usersize);
        if (newsd < 0){
                printf("Error in accept");
        }
        printf("Conexion aceptada de IP: %s\nPuerto: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        treatRequest(newsd);

    }
    close(sd);

    
    return 0;
}