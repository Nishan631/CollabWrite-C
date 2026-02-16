#include "text_buffer.h"
#include "editoperation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TextBuffer* createBuffer() {
    TextBuffer *buffer = (TextBuffer*)malloc(sizeof(TextBuffer));
    if (!buffer) {
        printf("Memory allocation failed for TextBuffer\n");
        exit(1);
    }
    buffer->head = buffer->tail = NULL;
    buffer->line_count = 0;
    buffer->undoStack = createStack();
    buffer->redoStack = createStack();
    return buffer;
}

LineNode* createLineNode(const char *text) {
    LineNode *newNode = (LineNode*)malloc(sizeof(LineNode));
    newNode->line = (char*)malloc(strlen(text) + 1);
    if (newNode->line == NULL) {
    printf("Memory allocation failed\n");
    exit(1);
    }
    strcpy(newNode->line, text);
    newNode->prev = newNode->next = NULL;
    return newNode;
}
void insertLine(TextBuffer *buffer, int position, const char *text) {
    if (position < 0 || position > buffer->line_count) return;
    LineNode *newNode = createLineNode(text);
    if (buffer->line_count == 0)
    {
    buffer->head = buffer->tail = newNode;
    }
    else if (position == 0) 
    {
    newNode->next = buffer->head;
    buffer->head->prev = newNode; 
    buffer->head = newNode; 
    }
    else if (position == buffer->line_count) 
    { buffer->tail->next = newNode; 
      newNode->prev = buffer->tail; 
      buffer->tail = newNode; 
    }
    else { 
        LineNode *curr = buffer->head; 
        for(int i=0;i<position-1;i++){
        curr=curr->next;
        }
        newNode->next=curr->next; 
        newNode->prev=curr; 
        curr->next->prev=newNode; 
        curr->next=newNode; 
    }

    buffer->line_count++;

    EditOperation *op = (EditOperation*)malloc(sizeof(EditOperation));
    op->type = INSERT_OP;
    op->position = position;
    op->oldText = NULL;
    op->newText = (char*)malloc(strlen(text)+1); strcpy(op->newText, text);
    pushOperation(buffer->undoStack, op);

    freeStack(buffer->redoStack);
    buffer->redoStack = createStack();
}

void deleteLine(TextBuffer *buffer, int position) {
    if (position < 0 || position >= buffer->line_count) 
    return;

    LineNode *curr = buffer->head;
    for(int i=0;i<position;i++) curr=curr->next;

    EditOperation *op = (EditOperation*)malloc(sizeof(EditOperation));
    op->type = DELETE_OP;
    op->position = position;
    op->oldText = (char*)malloc(strlen(curr->line)+1); strcpy(op->oldText, curr->line);
    op->newText = NULL;
    pushOperation(buffer->undoStack, op);

    freeStack(buffer->redoStack);
    buffer->redoStack = createStack();

    if (curr->prev) curr->prev->next = curr->next;
    else buffer->head = curr->next;
    if (curr->next) curr->next->prev = curr->prev;
    else buffer->tail = curr->prev;
    free(curr->line); free(curr);
    buffer->line_count--;
}

void updateLine(TextBuffer *buffer, int position, const char *newText) {
    if (position < 0 || position >= buffer->line_count) return;

    LineNode *curr = buffer->head; 
    for(int i=0;i<position;i++) 
    curr=curr->next;

    EditOperation *op = (EditOperation*)malloc(sizeof(EditOperation));
    op->type = UPDATE_OP;
    op->position = position;
    op->oldText = (char*)malloc(strlen(curr->line)+1); strcpy(op->oldText, curr->line);
    op->newText = (char*)malloc(strlen(newText)+1); strcpy(op->newText, newText);
    pushOperation(buffer->undoStack, op);

    freeStack(buffer->redoStack);
    buffer->redoStack = createStack();

    free(curr->line);
    curr->line = (char*)malloc(strlen(newText)+1);
    strcpy(curr->line, newText);
}


void printBuffer(TextBuffer *buffer) {
    LineNode *curr = buffer->head;
    int lineNum = 0;
    while (curr) {
        printf("%d: %s\n", lineNum, curr->line);
        curr = curr->next;
        lineNum++;
    }
}

void freeBuffer(TextBuffer *buffer) {
    LineNode *curr = buffer->head;
    while (curr) {
        LineNode *temp = curr;
        curr = curr->next;
        free(temp->line);
        free(temp);
    }
    free(buffer);
}
void undo(TextBuffer *buffer) {
    if (isStackEmpty(buffer->undoStack)) {
        printf("Nothing to undo.\n");
        return;
    }

    EditOperation *op = popOperation(buffer->undoStack);

    switch(op->type) {
        case INSERT_OP: deleteLine(buffer, op->position); break;
        case DELETE_OP: insertLine(buffer, op->position, op->oldText); break;
        case UPDATE_OP: updateLine(buffer, op->position, op->oldText); break;
    }

    pushOperation(buffer->redoStack, op);
}

void redo(TextBuffer *buffer) {
    if (isStackEmpty(buffer->redoStack)) {
        printf("Nothing to redo.\n");
        return;
    }

    EditOperation *op = popOperation(buffer->redoStack);

    switch(op->type) {
        case INSERT_OP: insertLine(buffer, op->position, op->newText); break;
        case DELETE_OP: deleteLine(buffer, op->position); break;
        case UPDATE_OP: updateLine(buffer, op->position, op->newText); break;
    }

    pushOperation(buffer->undoStack, op);
}

char* buffer_to_string(TextBuffer *buffer) {
    if (!buffer) return NULL;
    size_t total = 1;
    LineNode *curr = buffer->head;
    while (curr) {
        total += strlen(curr->line) + 1; // +1 for '\n'
        curr = curr->next;
    }

    char *result = (char*)malloc(total);
    if (!result) return NULL;
    result[0] = '\0';

    curr = buffer->head;
    while (curr) {
        strcat(result, curr->line);
        strcat(result, "\n");
        curr = curr->next;
    }
    return result;
}

int valid_position(TextBuffer *buffer, int pos) {
    return buffer && pos >= 0 && pos <= buffer->line_count;
}


void clearBuffer(TextBuffer *buffer) {
    LineNode *curr = buffer->head;
    while (curr) {
        LineNode *tmp = curr;
        curr = curr->next;
        free(tmp->line);
        free(tmp);
    }
    buffer->head = buffer->tail = NULL;
    buffer->line_count = 0;

    freeStack(buffer->undoStack);
    freeStack(buffer->redoStack);
    buffer->undoStack = createStack();
    buffer->redoStack = createStack();
}