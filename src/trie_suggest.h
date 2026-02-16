#ifndef TRIE_SUGGEST_H
#define TRIE_SUGGEST_H

#include <stddef.h>

#define ALPHABET_SIZE 26

typedef struct Suggestion {
    char *word;       
    int freq;         
    int dist;         
} Suggestion;
typedef struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE];
    int is_word;
    int freq;            
    char *word;          
} TrieNode;
void trie_init(void);

void trie_free(void);

void trie_insert(const char *word, int freq);

TrieNode *get_trie_root();
Suggestion **get_autocomplete(const char *prefix, int k, int *out_count);


Suggestion **get_autocorrect(const char *word, int max_dist, int k, int *out_count);


void free_suggestions_array(Suggestion **arr, int count);

int trie_contains(const char *word);

#endif 