#include "banker.h"
#include <stdio.h>
#include <string.h>
#define P 5
#define R 3
static void analyze(const char *name,int avail[R],int alloc[P][R],int max[P][R]){int need[P][R],work[R],finish[P]={0},seq[P],nseq=0;for(int r=0;r<R;r++)work[r]=avail[r];for(int p=0;p<P;p++)for(int r=0;r<R;r++)need[p][r]=max[p][r]-alloc[p][r];
 printf("\nBanker's analysis: %s\nAvailable: [%d %d %d]\n",name,avail[0],avail[1],avail[2]);puts("Need matrix (Maximum - Allocation):");for(int p=0;p<P;p++)printf("P%d: [%d %d %d]\n",p,need[p][0],need[p][1],need[p][2]);
 int progress;do{progress=0;for(int p=0;p<P;p++)if(!finish[p]&&need[p][0]<=work[0]&&need[p][1]<=work[1]&&need[p][2]<=work[2]){for(int r=0;r<R;r++)work[r]+=alloc[p][r];finish[p]=1;seq[nseq++]=p;progress=1;}}while(progress);
 if(nseq==P){printf("SAFE state. Safe sequence: ");for(int i=0;i<P;i++)printf("P%d%s",seq[i],i==P-1?"\n":" -> ");}else{printf("UNSAFE state. Processes unable to finish: ");for(int p=0;p<P;p++)if(!finish[p])printf("P%d ",p);puts("");}}
void run_banker(const char *which){int a[P][R]={{0,1,0},{2,0,0},{3,0,2},{2,1,1},{0,0,2}};int m[P][R]={{7,5,3},{3,2,2},{9,0,2},{2,2,2},{4,3,3}};int safe[R]={3,3,2},unsafe[R]={0,0,0};if(!strcmp(which,"safe")||!strcmp(which,"both"))analyze("safe test",safe,a,m);if(!strcmp(which,"unsafe")||!strcmp(which,"both"))analyze("unsafe test",unsafe,a,m);if(strcmp(which,"safe")&&strcmp(which,"unsafe")&&strcmp(which,"both"))puts("Usage: campuscompute banker [safe|unsafe|both]");}
