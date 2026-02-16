#ifndef SERVER_H
#define SERVER_H

#include "network.h"   
#include <pthread.h>

#include "trie_suggest.h"

typedef struct Client {
    int sock;
    struct Client *next;
} Client;

extern Client *clients;
extern pthread_mutex_t clients_mutex;
extern pthread_mutex_t buf_mutex;

void add_client(int sock);
void remove_client(int sock);
void broadcast(const char *msg);


void handle_autocomplete_request(int client_sock, const char *prefix);
void handle_autocorrect_request(int client_sock, const char *word);


void load_dictionary(const char *filename);

#endif