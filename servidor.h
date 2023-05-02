// Declaration of variables and functions for a singly linked list
#ifndef SERVIDOR_H
#define SERVIDOR_H

int init();
int set_value(int key, char* value1, int value2, double value3); //return 0 on success, -1 fail. writer
int get_value(int key, char* value1, int *value2, double *value3); //the values are returned to the pointers, reader
int modify_value(int key, char* value1, int value2, double value3); //writer
int delete_key(int key);//return 0 on success, -1 fail or key doesn't exist. writer
int exist(int key);//return 1 if key exists, 0 if it doesn't and -1 if there's an error reader
int copy_key(int key1, int key2);//Create and insert key2 with the values of key1, if key1 doesn't exist, return -1. if key2 exists, modify its values. return 0 on success.
void printList();
#endif
