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
}


void treatRequest(int newsd){ 
    char buf[MAX];
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
        char reply[MAX];

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
    }
    else{
        printf("Invalid command\n");
        char reply[MAX];
        strcpy(reply, "server said ERROR to your registration");
        if (socketSendMessage(newsd, reply, strlen(reply) + 1) < 0) {
            perror("Error in send");
            exit(1);
        }
    }
    printf("List of clients:\n\n");
    printList();
    close(newsd);
    printf("Connection closed\n\n");
}






int main(int argc , char *argv[]){

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

    socklen_t addrLen = sizeof(server_addr);
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