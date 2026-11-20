#include "resource_limits.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
void run_resource_limit(void){int fds[128],n=0,report[2];if(pipe(report)<0){perror("pipe");return;}pid_t p=fork();if(p<0){perror("fork");close(report[0]);close(report[1]);return;}if(p==0){close(report[0]);struct rlimit lim;if(getrlimit(RLIMIT_NOFILE,&lim)<0)_exit(2);lim.rlim_cur=16;if(lim.rlim_max<lim.rlim_cur)lim.rlim_cur=lim.rlim_max;if(setrlimit(RLIMIT_NOFILE,&lim)<0)_exit(3);while(n<128){int fd=open("/dev/null",O_RDONLY);if(fd<0)break;fds[n++]=fd;}int e=errno;write(report[1],&n,sizeof n);write(report[1],&e,sizeof e);for(int i=0;i<n;i++)close(fds[i]);_exit(0);}close(report[1]);int opened=-1,err=0;read(report[0],&opened,sizeof opened);read(report[0],&err,sizeof err);close(report[0]);int st;waitpid(p,&st,0);printf("Child applied RLIMIT_NOFILE soft limit = 16.\nOpen descriptors acquired before failure: %d\n",opened);if(err==EMFILE)puts("Observed effect: open() failed with EMFILE (per-process file descriptor limit reached).");else printf("Observed open() error: %d\n",err);if(WIFEXITED(st))printf("Resource-limit child exit status: %d\n",WEXITSTATUS(st));}
