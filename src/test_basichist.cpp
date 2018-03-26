#include <stdlib.h>
#include <stdio.h>
#include "basichist.h"



int main()
{
  int start, end;
  
  int lens[] ={1,1,2,2,2,3,4,5,5,6,
               6,7,8,8,9,10,10,11,11,12,
               13,13,13,14,14,15,15,16,19,18,
               20,30,33,33,33,40,41,41,42,43};
  
  basic_hist_t *bhp = build_basic_hist(40, lens, NULL);
  
  for (int i = 0; i< 40; i++){
    printf("(%d|%d) ", i,lens[i]);
  }
  
  printf("\nHIST: ");
  
  for (int i = 0; i< bhp->bucks_num; i++)
  {
    printf(" (%d|%d|%d)", bhp->buck_keys[i], bhp->buck_freq[i], bhp->buck_pos[i]);
  }
  
  printf("\n");
  
  int t;

  t = search_range_basic_hist(bhp, 17, 38, &start, &end);
  printf("Test range 17-38 = %d  [%d --- %d]\n", t, start, end);

  t = search_range_basic_hist(bhp, 5, 21, &start, &end);
  printf("Test range 5-21 = %d  [%d --- %d]\n", t, start, end);

  t = search_range_basic_hist(bhp, 12, 90, &start, &end);
  printf("Test range 12-90 = %d  [%d --- %d]\n", search_range_basic_hist(bhp, 12, 90, &start, &end), start, end);

  
  t= search_range_basic_hist(bhp, 1, 5, &start, &end);
  printf("Test range 1-5 = %d  [%d --- %d]\n", t, start, end);


  
  return 0;
}









  





