#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

// pointer to the bottom of the fixed size stack
#define USER_STACK_BOTTOM ((void*)0x08048000)
//file descriptor for the stdout
#define STDOUT_FILENO 1
#define STDIN_FILENO 0

void syscall_init (void);

void exit(int);
int write(int, const void*, unsigned);
#endif /* userprog/syscall.h */
