#include "trie_suggest.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>



static TrieNode *root = NULL;



static TrieNode *new_node(void) {
    TrieNode *n = (TrieNode *)calloc(1, sizeof(TrieNode));
    return n;
}

void trie_init(void) {
    if (root != NULL) return;
    root = new_node();
}

static void free_node_recursive(TrieNode *n) {
    if (!n) return;
    for (int i = 0; i < ALPHABET_SIZE; ++i) free_node_recursive(n->children[i]);
    if (n->word) free(n->word);
    free(n);
}

void trie_free(void) {
    free_node_recursive(root);
    root = NULL;
}


static void normalize_word(const char *src, char *dst) {
    int i = 0;
    for (; *src && i < 1023; ++src) {
        if (isalpha((unsigned char)*src)) {
            dst[i++] = (char)tolower((unsigned char)*src);
        }
    }
    dst[i] = '\0';
}



void trie_insert(const char *word, int freq) {
    if (!root) trie_init();
    char buf[1024];
    normalize_word(word, buf);
    if (buf[0] == '\0') return;

    TrieNode *cur = root;
    for (const char *p = buf; *p; ++p) {
        int idx = *p - 'a';
        if (idx < 0 || idx >= ALPHABET_SIZE) continue;
        if (!cur->children[idx]) cur->children[idx] = new_node();
        cur = cur->children[idx];
    }
    cur->is_word = 1;
    if (cur->word) free(cur->word);
    cur->word = strdup(buf);
    cur->freq = (freq > 0) ? freq : (cur->freq + 1); 
}



typedef struct _Collector {
    Suggestion **arr;
    int capacity;
    int size;
} Collector;

static Collector *collector_new(int k) {
    Collector *c = (Collector *)malloc(sizeof(Collector));
    c->capacity = k;
    c->size = 0;
    c->arr = (Suggestion **)malloc(sizeof(Suggestion*) * k);
    return c;
}
static void collector_free(Collector *c) {
    if (!c) return;
    free(c->arr);
    free(c);
}


static void collector_add(Collector *c, const char *word, int freq, int dist) {
    if (!word) return;
   
    Suggestion *s = (Suggestion *)malloc(sizeof(Suggestion));
    s->word = strdup(word);
    s->freq = freq;
    s->dist = dist;
 
    if (c->size < c->capacity) {
        c->arr[c->size++] = s;
    } else {
       
        int worst = 0;
        for (int i = 1; i < c->size; ++i) {
            Suggestion *a = c->arr[i];
            Suggestion *b = c->arr[worst];
            if (a->freq < b->freq || (a->freq == b->freq && a->dist > b->dist)) worst = i;
        }

        Suggestion *worstS = c->arr[worst];
        if (s->freq > worstS->freq || (s->freq == worstS->freq && s->dist < worstS->dist)) {
         
            free(worstS->word);
            free(worstS);
            c->arr[worst] = s;
        } else {
            
            free(s->word);
            free(s);
        }
    }
}


static void collect_from_node(TrieNode *node, Collector *collector, int max_dist_unused) {
    if (!node) return;
    if (node->is_word) collector_add(collector, node->word, node->freq, 0);
    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        if (node->children[i]) collect_from_node(node->children[i], collector, max_dist_unused);
    }
}

Suggestion **get_autocomplete(const char *prefix, int k, int *out_count) {
    if (!root) trie_init();
    char buf[1024];
    normalize_word(prefix, buf);
    TrieNode *cur = root;
    for (const char *p = buf; *p; ++p) {
        int idx = *p - 'a';
        if (idx < 0 || idx >= ALPHABET_SIZE) { cur = NULL; break; }
        cur = cur ? cur->children[idx] : NULL;
    }
    Collector *col = collector_new(k);
    if (cur) collect_from_node(cur, col, 0);

    for (int i = 0; i < col->size; ++i) {
        for (int j = i+1; j < col->size; ++j) {
            Suggestion *a = col->arr[i];
            Suggestion *b = col->arr[j];
            if (b->freq > a->freq || (b->freq == a->freq && strcmp(b->word, a->word) < 0)) {
                Suggestion *tmp = col->arr[i];
                col->arr[i] = col->arr[j];
                col->arr[j] = tmp;
            }
        }
    }

    *out_count = col->size;
    Suggestion **res = (Suggestion **)malloc(sizeof(Suggestion*) * col->size);
    for (int i = 0; i < col->size; ++i) {
        res[i] = col->arr[i];
    }
 
    free(col->arr);
    free(col);
    return res;
}


static int min_int3(int a, int b, int c) {
    int m = (a < b) ? a : b;
    return (m < c) ? m : c;
}


static void traverse_fuzzy(TrieNode *node, char *prefix, int prefix_len,
                           const char *target, int target_len,
                           int *prev_row, int max_dist, Collector *collector) {
    if (!node) return;

 
    int *curr_row = (int *)malloc((target_len + 1) * sizeof(int));
    curr_row[0] = prefix_len;
    int bestInRow = curr_row[0];
    for (int j = 1; j <= target_len; ++j) {
        int insert_cost = curr_row[j-1] + 1;
        int delete_cost = prev_row[j] + 1;
        int replace_cost = prev_row[j-1] + (target[j-1] == prefix[prefix_len-1] ? 0 : 1);
        curr_row[j] = min_int3(insert_cost, delete_cost, replace_cost);
        if (curr_row[j] < bestInRow) bestInRow = curr_row[j];
    }

    if (node->is_word) {
        int dist = curr_row[target_len];
        if (dist <= max_dist) {
            collector_add(collector, node->word, node->freq, dist);
        }
    }

    int minPossible = curr_row[0];
    for (int j = 1; j <= target_len; ++j) if (curr_row[j] < minPossible) minPossible = curr_row[j];
    if (minPossible <= max_dist) {
 
        for (int c = 0; c < ALPHABET_SIZE; ++c) {
            if (node->children[c]) {
                prefix[prefix_len] = 'a' + c;
                traverse_fuzzy(node->children[c], prefix, prefix_len + 1, target, target_len, curr_row, max_dist, collector);
            }
        }
    }

    free(curr_row);
}

Suggestion **get_autocorrect(const char *word, int max_dist, int k, int *out_count) {
    if (!root) trie_init();
    char target_buf[1024];
    normalize_word(word, target_buf);
    int tlen = strlen(target_buf);
  
    if (tlen == 0) { *out_count = 0; return NULL; }

    Collector *col = collector_new(k);

    int *initial_row = (int *)malloc((tlen + 1) * sizeof(int));
    for (int j = 0; j <= tlen; ++j) initial_row[j] = j;

    char prefix_buf[1024];

    for (int c = 0; c < ALPHABET_SIZE; ++c) {
        if (root->children[c]) {
            prefix_buf[0] = 'a' + c;
            traverse_fuzzy(root->children[c], prefix_buf, 1, target_buf, tlen, initial_row, max_dist, col);
        }
    }

    free(initial_row);

    for (int i = 0; i < col->size; ++i) {
        for (int j = i+1; j < col->size; ++j) {
            Suggestion *a = col->arr[i];
            Suggestion *b = col->arr[j];
            if (a->dist > b->dist || (a->dist == b->dist && a->freq < b->freq)) {
                Suggestion *tmp = col->arr[i];
                col->arr[i] = col->arr[j];
                col->arr[j] = tmp;
            }
        }
    }

    *out_count = col->size;
    Suggestion **res = NULL;
    if (col->size > 0) {
        res = (Suggestion **)malloc(sizeof(Suggestion*) * col->size);
        for (int i = 0; i < col->size; ++i) res[i] = col->arr[i];
    }

    free(col->arr);
    free(col);
    return res;
}

void free_suggestions_array(Suggestion **arr, int count) {
    if (!arr) return;
    for (int i = 0; i < count; ++i) {
        if (arr[i]) {
            if (arr[i]->word) free(arr[i]->word);
            free(arr[i]);
        }
    }
    free(arr);
}
TrieNode *get_trie_root() {
    return root;   
}

int trie_contains(const char *word) {
    if (!root) return 0;
    char buf[1024];
    normalize_word(word, buf);
    if (buf[0] == '\0') return 0;
    TrieNode *cur = root;
    for (const char *p = buf; *p; ++p) {
        int idx = *p - 'a';
        if (idx < 0 || idx >= ALPHABET_SIZE) return 0;
        if (!cur->children[idx]) return 0;
        cur = cur->children[idx];
    }
    return cur->is_word ? 1 : 0;
}