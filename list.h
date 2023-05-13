#ifndef LIST_H
#define LIST_H

//the definition of the list is in server.c
extern struct clientList *clntList;

struct ClientNode{

	char* username;
    char* alias;
    char* birthdate;
    //status can be 1 for online, 0 for offline
    int status;
    char* ip;
    int port;
    unsigned int lastMessageId;
    struct PendingMessageList *pendingMsgList;
	struct ClientNode *next;
    struct ClientNode *prev;
};

struct clientList{
    struct ClientNode *head;
    struct ClientNode *tail;
    int size;
};


//implementing pending message list
struct PendingMessageNode{
    unsigned int id;
    //the message size is controlled bt client
    char* message;
    char* aliasSender;
    char* aliasReceiver;
    struct PendingMessageNode *prev;
    struct PendingMessageNode *next;
};



struct PendingMessageList{
    struct PendingMessageNode *head;
    struct PendingMessageNode *tail;
    int size;
};

#endif