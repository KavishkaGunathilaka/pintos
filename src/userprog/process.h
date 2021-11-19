#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

//dynamic array for arguments
typedef struct {
  char** args;
  size_t used;
  size_t size;
} Argv;

void initArgv(Argv*, size_t);
void freeArray(Argv*);
void insertArgv(Argv*, char*);

#endif /* userprog/process.h */
