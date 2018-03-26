#ifndef __usage_h__
#define __usage_h__

#include <sys/resource.h>
#include <sys/time.h>


typedef struct __usage_timer_t
{
  struct rusage start_r;
  struct timeval start_t;
  
  struct rusage end_r;
  struct timeval end_t;


  long user_time_sec;
  long user_time_usec;
  long sys_time_sec;
  long sys_time_usec;
  long elapse_time_sec;
  long elapse_time_usec;

} usage_timer_t;


void ResetUsage(usage_timer_t *up);

void StatUsage(usage_timer_t *up);

void ShowUsage(usage_timer_t *up, const char *Prefix, FILE* fp);



#endif
