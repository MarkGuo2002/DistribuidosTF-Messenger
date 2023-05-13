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
    unregister(clntUsername);
    printList();

    unregister("Hello");
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

    
    
    return 0;
}