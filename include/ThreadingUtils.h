#ifndef THREADINGUTILS_H
#define THREADINGUTILS_H

typedef struct {
    int banchoIndex;
    int taskIndex;
} BanchoArgs;

int getFreeBanchoIndex();
int getFreeTaskIndex();

#endif
