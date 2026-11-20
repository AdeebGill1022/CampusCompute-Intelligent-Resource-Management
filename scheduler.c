#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct { int id, arrival, burst, priority, remaining, start, completion; } Job;
typedef struct { int id, start, end; } Slice;
static Job sample[] = {{1,0,5,2,5,-1,0},{2,1,3,1,3,-1,0},{3,2,8,3,8,-1,0},{4,3,6,2,6,-1,0}};
static int read_int(const char *prompt, int min, int max, int *out) {
    char line[128], extra; long v;
    printf("%s", prompt); fflush(stdout);
    if (!fgets(line,sizeof line,stdin) || sscanf(line,"%ld %c",&v,&extra)!=1 || v<min || v>max) return 0;
    *out=(int)v; return 1;
}
static int load_jobs(Job j[], int custom) {
    int n=4;
    if (!custom) { memcpy(j,sample,sizeof sample); return n; }
    if (!read_int("Number of jobs (1-20): ",1,20,&n)) { puts("Error: enter an integer from 1 to 20."); return 0; }
    for(int i=0;i<n;i++) {
        char p[100]; j[i]=(Job){0};
        snprintf(p,sizeof p,"Job %d arrival time (0-100000): ",i+1); if(!read_int(p,0,100000,&j[i].arrival)) goto bad;
        snprintf(p,sizeof p,"Job %d CPU burst (1-100000): ",i+1); if(!read_int(p,1,100000,&j[i].burst)) goto bad;
        snprintf(p,sizeof p,"Job %d priority (1-100000; lower is higher): ",i+1); if(!read_int(p,1,100000,&j[i].priority)) goto bad;
        j[i].id=i+1; j[i].remaining=j[i].burst; j[i].start=-1;
    }
    return n;
bad: puts("Error: malformed or out-of-range job data; scheduling cancelled."); return 0;
}
static void print_metrics(Job j[], int n) {
    double tw=0,tt=0;
    puts("\nJob  Arrival  Burst  Priority  Completion  Turnaround  Waiting");
    for(int i=0;i<n;i++){ int tat=j[i].completion-j[i].arrival, wt=tat-j[i].burst; tw+=wt;tt+=tat;
        printf("J%-3d %-8d %-6d %-9d %-11d %-11d %d\n",j[i].id,j[i].arrival,j[i].burst,j[i].priority,j[i].completion,tat,wt); }
    printf("Average turnaround: %.2f\nAverage waiting: %.2f\n",tt/n,tw/n);
}
static void schedule_fcfs(Job j[],int n) {
    int done[20]={0},t=0,finished=0; Slice s[200];int ns=0;
    while(finished<n){int k=-1;for(int i=0;i<n;i++)if(!done[i]&&j[i].arrival<=t&&(k<0||j[i].arrival<j[k].arrival||(j[i].arrival==j[k].arrival&&j[i].id<j[k].id)))k=i;
        if(k<0){int a=INT_MAX;for(int i=0;i<n;i++)if(!done[i]&&j[i].arrival<a)a=j[i].arrival;t=a;continue;}
        j[k].start=t;s[ns++]=(Slice){j[k].id,t,t+j[k].burst};t+=j[k].burst;j[k].completion=t;done[k]=1;finished++;}
    puts("Execution timeline (FCFS; ties: earlier arrival, then lower job ID):");for(int i=0;i<ns;i++)printf("[J%d: %d-%d] ",s[i].id,s[i].start,s[i].end);puts("");print_metrics(j,n);
}
static void schedule_priority(Job j[],int n) {
    int done[20]={0},t=0,finished=0;Slice s[200];int ns=0;
    while(finished<n){int k=-1;for(int i=0;i<n;i++)if(!done[i]&&j[i].arrival<=t&&(k<0||j[i].priority<j[k].priority||(j[i].priority==j[k].priority&&(j[i].arrival<j[k].arrival||(j[i].arrival==j[k].arrival&&j[i].id<j[k].id)))))k=i;
        if(k<0){int a=INT_MAX;for(int i=0;i<n;i++)if(!done[i]&&j[i].arrival<a)a=j[i].arrival;t=a;continue;}
        j[k].start=t;s[ns++]=(Slice){j[k].id,t,t+j[k].burst};t+=j[k].burst;j[k].completion=t;done[k]=1;finished++;}
    puts("Execution timeline (non-preemptive priority; lower number = higher priority; ties: arrival then job ID):");for(int i=0;i<ns;i++)printf("[J%d: %d-%d] ",s[i].id,s[i].start,s[i].end);puts("");print_metrics(j,n);
}
static void schedule_rr(Job j[],int n,int q) {
    int queue[32],head=0,tail=0,qcount=0,added[20]={0},done=0,t=0;
    int total=0; for(int i=0;i<n;i++) total+=j[i].burst;
    Slice *s=calloc((size_t)total+1,sizeof(*s)); if(!s){perror("calloc");return;} int ns=0;
    while(done<n){
        for(int i=0;i<n;i++)if(!added[i]&&j[i].arrival<=t){queue[tail]=i;tail=(tail+1)%32;qcount++;added[i]=1;}
        if(qcount==0){int a=INT_MAX;for(int i=0;i<n;i++)if(!added[i]&&j[i].arrival<a)a=j[i].arrival;t=a;continue;}
        int k=queue[head];head=(head+1)%32;qcount--;
        int run=j[k].remaining<q?j[k].remaining:q;if(j[k].start<0)j[k].start=t;
        s[ns++]=(Slice){j[k].id,t,t+run};t+=run;j[k].remaining-=run;
        for(int i=0;i<n;i++)if(!added[i]&&j[i].arrival<=t){queue[tail]=i;tail=(tail+1)%32;qcount++;added[i]=1;}
        if(j[k].remaining>0){queue[tail]=k;tail=(tail+1)%32;qcount++;}else{j[k].completion=t;done++;}
    }
    printf("Execution timeline (Round Robin, time quantum = %d; equal-time arrivals enter queue before the preempted job):\n",q);for(int i=0;i<ns;i++)printf("[J%d: %d-%d] ",s[i].id,s[i].start,s[i].end);puts("");print_metrics(j,n);free(s);
}
void run_scheduler(const char *policy,int custom){Job j[20];int n=load_jobs(j,custom);if(!n)return;
    printf("\nJobs: %d\n",n);if(!strcmp(policy,"fcfs"))schedule_fcfs(j,n);else if(!strcmp(policy,"priority"))schedule_priority(j,n);else if(!strcmp(policy,"rr")){int q;if(!read_int("Round Robin time quantum (1-100000): ",1,100000,&q)){puts("Error: invalid time quantum.");return;}schedule_rr(j,n,q);}else puts("Unknown policy. Use fcfs, rr, or priority.");}
