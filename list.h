#ifndef LIST_H
#define LIST_H



struct clientList *clntList;


struct ClientNode{

	char* username;
    char* alias;
    char* birthdate;
    //status can be 1 for online, 0 for offline
    int status;
    char* ip;
    int port;
    struct PendingMessageList *pendingMsgList;
    unsigned int lastMessageId;
	struct ClientNode *next;
};


//implementing pending message list
struct PendingMessageNode{
    unsigned int id;
    //the message size is controlled bt client
    char* message;
    char* aliasSender;
    char* aliasReceiver;
    struct PendingMessageNode *next;
};

struct clientList{
    struct ClientNode *head;
    struct ClientNode *tail;
    int size;
};


struct PendingMessageList{
    struct PendingMessageNode *head;
    struct PendingMessageNode *tail;
    int size;
};

#endif