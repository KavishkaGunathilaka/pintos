#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"
#include "process.h"
#include "filesys/filesys.h"
#include <string.h>

static void syscall_handler (struct intr_frame *);
static bool validatePointer(const void*);
static bool validateBuffer(const void*, unsigned);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init(&fileSysLock);
}

static void
syscall_handler (struct intr_frame *f) 
{
  //printf ("system call!\n");
  if (validatePointer((void*)f->esp)){
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
      case SYS_HALT:{
        halt();
        break;
      }
      case SYS_EXEC:{
        char* fileName = (*((int*)f->esp + 1));
        f->eax = exec(fileName);
        break;
      }
      case SYS_WAIT:{
        pid_t pid = *((int*)f->esp + 1);
        f->eax = wait(pid);
        break;
      }
      case SYS_CREATE:{
        char* file = (char*)(*((int*)f->esp + 1));
        unsigned size = *((unsigned*)f->esp + 2);
        f->eax = create(file, size);
        break;
      }
      case SYS_REMOVE:{
        char* file = (char*)(*((int*)f->esp + 1));
        f->eax = remove(file);
        break;
      }
      case SYS_OPEN:{
        char* file = (char*)(*((int*)f->esp + 1));
        f->eax = open(file);
        break;
      }
      case SYS_FILESIZE:{
        break;
      }
      case SYS_READ:{
        break;
      }
      case SYS_SEEK:{
        break;
      }
      case SYS_TELL:{
        break;
      }
      case SYS_CLOSE:{
        break;
      }
    }
  } else {
    exit(-1);
  }
}

/* Checks the validity of the provided pointer */
static bool validatePointer(const void* ptr){
  //checking if the address is in user space and within the stack
  if (is_user_vaddr(ptr) && (ptr > USER_STACK_BOTTOM)){
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
      return false;
    };
  }
  return true;
}

void exit(int status){
  printf("%s: exit(%d)\n", thread_current()->name, status);
  thread_current()->parent->childStatus = status;
  thread_exit();
}

int write(int fd, const void *buffer, unsigned size){
  //printf ("system call write!\n");
  if (validateBuffer(buffer, size)){
    if (fd == STDOUT_FILENO){
      putbuf(buffer, size);
      return size;
    } else {
      printf("Write to the file\n");
    }
  } else {
    exit(-1);
  }
}

void halt(){
  shutdown_power_off();
}

pid_t exec(const char* cmd_line){
  return process_execute(cmd_line);
}

int wait(pid_t pid){
  return process_wait(pid);
}

bool create(const char* file, unsigned size){
  if (validatePointer(file) && (file != NULL) && strlen(file)>0 ){
    lock_acquire(&fileSysLock);
    bool result = filesys_create(file, size);
    lock_release(&fileSysLock);
    return result;
  } else {
    exit(-1);
  }
}

bool remove(const char* file){
  if (validatePointer(file) && (file != NULL) && strlen(file)>0 ){
    lock_acquire(&fileSysLock);
    bool result = filesys_remove(file);
    lock_release(&fileSysLock);
    return result;
  } else {
    exit(-1);
  }
}

int open(const char* file){
  if (validatePointer(file) && (file != NULL)){
    lock_acquire(&fileSysLock);
    struct file* openedFile = filesys_open(file);
    thread_current()->fileDes[thread_current()->nextFD++] = openedFile;
    lock_release(&fileSysLock);
    return openedFile? ((thread_current()->nextFD) - 1): -1;
  } else {
    exit(-1);
  }
}