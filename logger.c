#include "logger.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define QCAP 5
#define LOGFILE "logs/campuscompute.log"
typedef struct { char text[160]; } Event;
static Event q[QCAP]; static int head,tail,count,closed; static pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;static pthread_cond_t not_empty=PTHREAD_COND_INITIALIZER,not_full=PTHREAD_COND_INITIALIZER;static FILE *fp;
static void *producer(void *arg){int n=*(int*)arg;for(int i=1;i<=n;i++){Event e;snprintf(e.text,sizeof e.text,"event %02d: job J%d submitted by producer",i,i);
 pthread_mutex_lock(&lock);while(count==QCAP)pthread_cond_wait(&not_full,&lock);q[tail]=e;tail=(tail+1)%QCAP;count++;printf("PRODUCER queued: %s\n",e.text);pthread_cond_signal(&not_empty);pthread_mutex_unlock(&lock);}
 pthread_mutex_lock(&lock);closed=1;pthread_cond_broadcast(&not_empty);pthread_mutex_unlock(&lock);return NULL;}
static void *consumer(void *unused){(void)unused;for(;;){pthread_mutex_lock(&lock);while(count==0&&!closed)pthread_cond_wait(&not_empty,&lock);if(count==0&&closed){pthread_mutex_unlock(&lock);break;}Event e=q[head];head=(head+1)%QCAP;count--;pthread_cond_signal(&not_full);pthread_mutex_unlock(&lock);
 time_t now=time(NULL);struct tm tmv;localtime_r(&now,&tmv);char stamp[32];strftime(stamp,sizeof stamp,"%Y-%m-%d %H:%M:%S",&tmv);fprintf(fp,"[%s] %s\n",stamp,e.text);fflush(fp);printf("CONSUMER logged: %s\n",e.text);}return NULL;}
void run_logger(int n){if(n<1||n>100){puts("Error: logger event count must be 1-100.");return;}if(system("mkdir -p logs")!=0){puts("Error: cannot create logs directory.");return;}fp=fopen(LOGFILE,"a");if(!fp){perror("fopen log");return;}head=tail=count=closed=0;pthread_t p,c;
 if(pthread_create(&c,NULL,consumer,NULL)){puts("Error: consumer thread creation failed.");fclose(fp);return;}if(pthread_create(&p,NULL,producer,&n)){puts("Error: producer thread creation failed.");pthread_mutex_lock(&lock);closed=1;pthread_cond_broadcast(&not_empty);pthread_mutex_unlock(&lock);pthread_join(c,NULL);fclose(fp);return;}
 pthread_join(p,NULL);pthread_join(c,NULL);fclose(fp);puts("Logger: producer and consumer threads joined; queue drained and log closed.");}
