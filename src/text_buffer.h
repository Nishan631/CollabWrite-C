#ifndef TEXT_BUFFER_H
#define TEXT_BUFFER_H

#include "editoperation.h"


typedef struct LineNode {
    char *line;
    struct LineNode *prev;
    struct LineNode *next;
} LineNode;

typedef struct TextBuffer {
    LineNode *head;
    LineNode *tail;
    int line_count;
    OperationStack *undoStack;  
    OperationStack *redoStack;  
} TextBuffer;

TextBuffer* createBuffer();
LineNode* createLineNode(const char *text);
void insertLine(TextBuffer *buffer, int position, const char *text);
void deleteLine(TextBuffer *buffer, int position);
void updateLine(TextBuffer *buffer, int position, const char *newText);
void printBuffer(TextBuffer *buffer);
void freeBuffer(TextBuffer *buffer);
void undo(TextBuffer *buffer);
void redo(TextBuffer *buffer);


char* buffer_to_string(TextBuffer *buffer);
int valid_position(TextBuffer *buffer, int pos);
void clearBuffer(TextBuffer *buffer);

#endif