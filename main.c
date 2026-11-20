#include "scheduler.h"
#include "logger.h"
#include "ipc.h"
#include "banker.h"
#include "worker.h"
#include "resource_limits.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static void help(const char *p){printf("CampusCompute - OS concepts prototype\nUsage: %s COMMAND [OPTIONS]\n\nCommands:\n  help                    Show this help\n  schedule POLICY         POLICY: fcfs | rr | priority (sample jobs)\n  schedule custom POLICY Enter validated jobs interactively\n  compare                 Run all three policies on the same sample jobs\n  logger [COUNT]          Run producer-consumer logger (default 8 events)\n  ipc                     Shared memory + named semaphore demonstration\n  banker [safe|unsafe|both] Banker's safety analysis (default both)\n  worker [normal|fail|both] fork/waitpid demonstration (default both)\n  limit                   Demonstrate RLIMIT_NOFILE in a child process\n  demo                    Run an integrated demonstration\n  logs                    Display the generated activity log\n\nScheduling sample: J1(AT=0,BT=5,Pr=2), J2(1,3,1), J3(2,8,3), J4(3,6,2).\nPriority: lower numeric value means higher priority. RR asks for its time quantum.\n",p);}
static void compare(void){puts("\n========== FCFS ==========");run_scheduler("fcfs",0);puts("\n========== ROUND ROBIN ==========");puts("Enter a time quantum to compare on the same workload:");run_scheduler("rr",0);puts("\n========== PRIORITY ==========");run_scheduler("priority",0);}
static void integrated(void){puts("\n===== CampusCompute integrated demonstration =====");run_scheduler("fcfs",0);puts("\n[1/6] Producer-consumer logger");run_logger(6);puts("\n[2/6] Shared-memory IPC");run_ipc();puts("\n[3/6] Banker's safe and unsafe cases");run_banker("both");puts("\n[4/6] Normal and simulated-failure workers");run_worker("both");puts("\n[5/6] Process resource limit");run_resource_limit();puts("\n[6/6] Integrated demonstration complete.");}
int main(int argc,char **argv){
    if(argc<2){help(argv[0]);return 0;}
    if(!strcmp(argv[1],"help")||!strcmp(argv[1],"--help")){help(argv[0]);return 0;}
    if(!strcmp(argv[1],"schedule")){
        if(argc==3)run_scheduler(argv[2],0);
        else if(argc==4&&!strcmp(argv[2],"custom"))run_scheduler(argv[3],1);
        else{fprintf(stderr,"Usage: %s schedule [custom] [fcfs|rr|priority]\n",argv[0]);return 2;}
    } else if(!strcmp(argv[1],"compare"))compare();
    else if(!strcmp(argv[1],"logger")){
        int n=8;
        if(argc>2){char *e;long v=strtol(argv[2],&e,10);if(*e||v<1||v>100){fprintf(stderr,"Error: COUNT must be an integer from 1 to 100.\n");return 2;}n=(int)v;}
        run_logger(n);
    } else if(!strcmp(argv[1],"ipc"))run_ipc();
    else if(!strcmp(argv[1],"banker"))run_banker(argc>2?argv[2]:"both");
    else if(!strcmp(argv[1],"worker"))run_worker(argc>2?argv[2]:"both");
    else if(!strcmp(argv[1],"limit"))run_resource_limit();
    else if(!strcmp(argv[1],"demo"))integrated();
    else if(!strcmp(argv[1],"logs")){
        FILE *f=fopen("logs/campuscompute.log","r");
        if(!f){perror("logs/campuscompute.log");return 1;}
        char b[256];while(fgets(b,sizeof b,f))fputs(b,stdout);fclose(f);
    } else {fprintf(stderr,"Error: unknown command '%s'.\n",argv[1]);help(argv[0]);return 2;}
    return 0;
}
