#include "ipc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <errno.h>
#include <sys/stat.h>
typedef struct { int ready; char message[256]; } Shared;
void run_ipc(void){char shmname[80],semname[80];snprintf(shmname,sizeof shmname,"/campuscompute_shm_%ld",(long)getpid());snprintf(semname,sizeof semname,"/campuscompute_sem_%ld",(long)getpid());int fd=-1,shm_created=0,sem_created=0;Shared *sh=MAP_FAILED;sem_t *sem=SEM_FAILED;pid_t pid=-1;int status=0;
 fd=shm_open(shmname,O_CREAT|O_EXCL|O_RDWR,0600);if(fd<0){perror("shm_open");goto cleanup;}shm_created=1;if(ftruncate(fd,sizeof(Shared))<0){perror("ftruncate");goto cleanup;}sh=mmap(NULL,sizeof(Shared),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);if(sh==MAP_FAILED){perror("mmap");goto cleanup;}memset(sh,0,sizeof *sh);
 sem=sem_open(semname,O_CREAT|O_EXCL,0600,0);if(sem==SEM_FAILED){perror("sem_open");goto cleanup;}sem_created=1;pid=fork();if(pid<0){perror("fork");goto cleanup;}if(pid==0){snprintf(sh->message,sizeof sh->message,"Hello from child PID %ld via POSIX shared memory",(long)getpid());sh->ready=1;sem_post(sem);_exit(0);}if(waitpid(pid,&status,0)<0){perror("waitpid");pid=-1;goto cleanup;}if(sem_wait(sem)<0){perror("sem_wait");goto cleanup;}printf("Parent received shared-memory message: %s\n",sh->message);printf("Shared-memory ready flag: %d\n",sh->ready);if(WIFEXITED(status))printf("IPC child exit status: %d\n",WEXITSTATUS(status));
cleanup:if(pid>0 && (!WIFEXITED(status)&&!WIFSIGNALED(status)))waitpid(pid,&status,0);if(sem!=SEM_FAILED)sem_close(sem);if(sem_created)sem_unlink(semname);if(sh!=MAP_FAILED)munmap(sh,sizeof(Shared));if(fd>=0)close(fd);if(shm_created)shm_unlink(shmname);puts("IPC cleanup: program-created semaphore and shared-memory object released.");}
