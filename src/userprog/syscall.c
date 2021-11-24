#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

static void syscall_handler (struct intr_frame *);
static bool validatePointer(const void*);
static bool validateBuffer(const void*, unsigned);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  //printf ("system call!\n");

  switch (*(int*)f->esp){
    case SYS_EXIT:{
      int status = *((int*)f->esp + 1);
      exit(status);
      break;
    }
    case SYS_WRITE: {
      int fd = *((int*)f->esp + 1);
      void* buffer = (void*)(*((int*)f->esp + 2));
      unsigned size = *((unsigned*)f->esp + 3);
      int writtenSize;
      if ((writtenSize = write(fd, buffer, size))<0){
        exit(-1);
      } else {
        f->eax = writtenSize;
      }
      break;
    }
  }
}

/* Checks the validity of the provided pointer */
static bool validatePointer(const void* ptr){
  //checking if the address is in user space and within the stack
  if (is_user_vaddr(ptr) && ptr > USER_STACK_BOTTOM){
    //checking if the address belongs to current process
    if (pagedir_get_page(thread_current()->pagedir, ptr)){
      return true;
    }
  }
  return false;
}

static bool validateBuffer(const void *buffer, unsigned size){
  for (int i = 0; i < size; i++){
    if (!validatePointer(&buffer[i])){
      //printf("%s\n", buffer);
      return false;
    };
  }
  return true;
}

void exit(int status){
  //printf ("system call exit!\n");
  printf("%s: exit(%u)\n", thread_current()->name, status);
  thread_exit();
}

int write(int fd, const void *buffer, unsigned size){
  //printf ("system call write!\n");
  if (validateBuffer(buffer, size)){
    if (fd == STDOUT_FILENO){
      //printf("Putting buffer to stdout\n");
      putbuf(buffer, size);
      return size;
    } else {
      printf("Write to the file\n");
    }
  } else {
    return -1;
  }
}
