#include <stdio.h>
#include "usage.h"

void ResetUsage(usage_timer_t *up)
{
  up->elapse_time_sec = up->elapse_time_usec = 0;
  up->sys_time_sec = up->sys_time_usec = 0;
  up->user_time_sec = up->user_time_usec = 0;
  
  getrusage(RUSAGE_SELF, &up->start_r);
  gettimeofday(&up->start_t, NULL);
}

void StatUsage(usage_timer_t *up)
{
  getrusage(RUSAGE_SELF, &up->end_r);
  gettimeofday(&up->end_t, NULL);
  
  if (up->end_t.tv_usec < up->start_t.tv_usec){
    up->end_t.tv_sec--;
    up->end_t.tv_usec += 1000000;
  }
  if (up->end_r.ru_utime.tv_usec < up->start_r.ru_utime.tv_usec){
    up->end_r.ru_utime.tv_sec--;
    up->end_r.ru_utime.tv_usec += 1000000;
  }
  if (up->end_r.ru_stime.tv_usec < up->start_r.ru_stime.tv_usec){
    up->end_r.ru_stime.tv_sec--;
    up->end_r.ru_stime.tv_usec += 1000000;
  }
  
  
  up->elapse_time_sec = (long) (up->end_t.tv_sec - up->start_t.tv_sec);
  up->elapse_time_usec = (long) (up->end_t.tv_usec - up->start_t.tv_usec);

  up->user_time_sec = (long) (up->end_r.ru_utime.tv_sec - up->start_r.ru_utime.tv_sec);
  up->user_time_usec = (long) (up->end_r.ru_utime.tv_usec - up->start_r.ru_utime.tv_usec);
  
  up->sys_time_sec = (long) (up->end_r.ru_stime.tv_sec - up->start_r.ru_stime.tv_sec);
  up->sys_time_usec = (long) (up->end_r.ru_stime.tv_usec - up->start_r.ru_stime.tv_usec);
  
}


void ShowUsage(usage_timer_t *up, const char *Prefix, FILE* fp)
{
  fprintf(fp,"%sTotalTime: %ld.%06ld\n",
          Prefix, up->elapse_time_sec, up->elapse_time_usec);
}
