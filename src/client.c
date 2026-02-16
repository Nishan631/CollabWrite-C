#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <ctype.h>

#include "network.h"
#include "trie_suggest.h"  

int sockfd;
int use_local_suggestions = 1; 

void *recv_thread(void *arg) {
    char buf[BUFSIZE * 2];
    ssize_t n;
    while ((n = recv(sockfd, buf, sizeof(buf) - 1, 0)) > 0) {
        buf[n] = '\0';
        printf("\n[SERVER] %s", buf);
        fflush(stdout);
    }
    printf("\nDisconnected from server.\n");
    exit(0);
    return NULL;
}

void try_autocomplete(const char *input) {
    if (!use_local_suggestions) return;

    char prefix[128];
    int len = strlen(input);
    int i = len - 1, j = 0;

    
    while (i >= 0 && !isspace((unsigned char)input[i])) i--;
    strncpy(prefix, input + i + 1, sizeof(prefix) - 1);
    prefix[sizeof(prefix) - 1] = 0;

    if (strlen(prefix) >= 2) {
        int n;
        Suggestion **s = get_autocomplete(prefix, 5, &n);
        if (n > 0) {
            printf("\nSuggestions for '%s': ", prefix);
            for (int k = 0; k < n; k++) {
                printf("%s ", s[k]->word);
            }
            printf("\n>> ");
            fflush(stdout);
        }
        free_suggestions_array(s, n);
    }
}

int main(int argc, char **argv) {
    const char *server_ip = "127.0.0.1";
    if (argc >= 2) server_ip = argv[1];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in serv = {0};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);
    inet_pton(AF_INET, server_ip, &serv.sin_addr);

    if (connect(sockfd, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
        perror("connect");
        return 1;
    }

    printf("Connected to server %s:%d\n", server_ip, PORT);
    pthread_t tid;
    pthread_create(&tid, NULL, recv_thread, NULL);

    if (use_local_suggestions) {
        trie_init();
        FILE *f = fopen("words.txt", "r");
        char buf[512];
        while (f && fgets(buf, sizeof(buf), f)) {
            buf[strcspn(buf, "\r\n")] = 0;
            trie_insert(buf, 1);
        }
        if (f) fclose(f);
    }

    printf("\nCommands:\n");
    printf("INS <pos> <text>      - insert line at pos (0-based)\n");
    printf("DEL <pos>             - delete line at pos\n");
    printf("UPD <pos> <text>      - update line at pos\n");
    printf("UNDO / REDO / SNAP / GET / PRINT / QUIT\n");
    printf("(Autocomplete active — start typing a word to see suggestions), COR for autocorrect\n\n");

    char line[BUFSIZE];

    while (1) {
        printf(">> ");
        if (!fgets(line, sizeof(line), stdin)) break;

        line[strcspn(line, "\r\n")] = 0;

        if (strncmp(line, "QUIT", 4) == 0) {
            close(sockfd);
            break;
        }
            
    if (strncmp(line, "COR", 3) == 0) {
        char *arg = line + 4; 
        int count = 0;
        Suggestion **arr = get_autocorrect(arg, 2, 5, &count);

        printf("\nAutocorrect suggestions for '%s':\n", arg);
        for (int i = 0; i < count; ++i) {
            printf("%s (dist=%d, freq=%d)\n", arr[i]->word, arr[i]->dist, arr[i]->freq);
        }

        free_suggestions_array(arr, count);

        continue; 
    }


        if (strncmp(line, "INS", 3) != 0 &&
    strncmp(line, "DEL", 3) != 0 &&
    strncmp(line, "UPD", 3) != 0 &&
    strncmp(line, "UNDO", 4) != 0 &&
    strncmp(line, "REDO", 4) != 0 &&
    strncmp(line, "SNAP", 4) != 0 &&
    strncmp(line, "GET", 3) != 0 &&
    strncmp(line, "PRINT", 5) != 0 &&
    strncmp(line, "COR", 3) != 0) {   
    try_autocomplete(line);            
}


        send(sockfd, line, strlen(line), 0);
        send(sockfd, "\n", 1, 0);
    }

    close(sockfd);
    return 0;
}