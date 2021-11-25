#include "lib/user/syscall.h"
#include "threads/synch.h"

#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

// pointer to the bottom of the fixed size stack
#define USER_STACK_BOTTOM ((void*)0x08048000)
//file descriptor for the stdout
#define STDOUT_FILENO 1
#define STDIN_FILENO 0

void syscall_init (void);

struct lock fileSysLock;

void exit(int);
int write(int, const void*, unsigned);
void halt(void);
pid_t exec(const char*);
int wait(pid_t);
bool create(const char*, unsigned);
bool remove(const char*);
int open(const char*);
#endif /* userprog/syscall.h */
