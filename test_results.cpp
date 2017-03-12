#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "verify.h"
#include "usage.h"


#define MAX_RESULT_NUM 1024*1024
  
char *stra[MAX_RESULT_NUM];
int slena[MAX_RESULT_NUM];
char *strb[MAX_RESULT_NUM];
int slenb[MAX_RESULT_NUM];
int eds[MAX_RESULT_NUM];
//int thres[MAX_RESULT_NUM];
int rstnum = 0;


int main(int argc, char **argv)
{
  int a,tau,ed,la,lb;
  char wst[10];
  
  usage_timer_t timer;
  if ( argc != 2 ){
    fprintf(stderr,"USAGE %s threshold < results\n", argv[0]);
    exit (-1);
  }
  
  tau = atoi(argv[1]);
  
  while ((fscanf(stdin, "RESULT: %d %d %d %d\n", &ed, &a,  &la, &lb)==4))
  {
    stra[rstnum] = new char [la+5];
    slena[rstnum] = la;
    strb[rstnum] = new char [lb+5];
    slenb[rstnum] = lb;
    eds[rstnum]= ed;
    //thres[rstnum] = thre;
    fgets(stra[rstnum], la+5, stdin);
    fgets(strb[rstnum], lb+5, stdin);
    fgets(wst, 10, stdin);
    
    stra[rstnum][la] = '\0';
    strb[rstnum][lb] = '\0';    
    rstnum ++;
  }

  ResetUsage(&timer);
  for (int i = 0; i < rstnum; i++)
  {
    if(eds[i] > tau) continue;
    ed = edit_distance(slena[i], stra[i], slenb[i], strb[i], tau);
    fprintf(stdout, "%d %d %d %s %s\n", ed, eds[i], tau, stra[i], strb[i]);
  }
  StatUsage(&timer);
  char msg[100];
  sprintf(msg, "VerifyTime Tau: %d ", tau);
  ShowUsage(&timer, msg , stderr);
  return 0;
}


