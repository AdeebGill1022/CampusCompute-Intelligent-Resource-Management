#include "worker.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
static void one(int fail){fflush(NULL);pid_t p=fork();if(p<0){perror("fork");return;}if(p==0){printf("Child worker PID %ld started (%s).\n",(long)getpid(),fail?"controlled simulated failure":"normal task");fflush(stdout);_exit(fail?42:0);}int st;if(waitpid(p,&st,0)<0){perror("waitpid");return;}if(WIFEXITED(st)){int c=WEXITSTATUS(st);printf("Parent collected PID %ld: exited with status %d (%s).\n",(long)p,c,c==0?"normal completion":(fail&&c==42?"controlled simulated failure":"non-zero exit"));}else if(WIFSIGNALED(st))printf("Parent collected PID %ld: terminated by signal %d.\n",(long)p,WTERMSIG(st));else puts("Parent collected child with an unclassified wait status.");}
void run_worker(const char *mode){if(!strcmp(mode,"normal"))one(0);else if(!strcmp(mode,"fail"))one(1);else if(!strcmp(mode,"both")){one(0);one(1);}else puts("Usage: campuscompute worker [normal|fail|both]");}
