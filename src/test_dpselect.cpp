#include <stdlib.h>
#include <string.h>
#include "dpselect.h"







void output_sel(int costs[], int costlen, int sel[], int tau, int q)
{
  fprintf(stdout, "SEL %d with q= %d: ", tau+1, q);
  for (int i = 0; i <costlen; i++)
  {
    for(int j = 0; j < tau + 1; j ++)
    {
      if (sel[j] == i)
      {
        fprintf(stdout, "*");
        break;
      }
    }
    fprintf(stdout, "(%d|%d) ", i, costs[i]);
  }
  fprintf(stdout, "\n");
}




// This tester generate a cost array and calculate
// the dpselect and output it.
int main()
{
  int costs[] = {
    12, 23, 99, 10, 11, 
    2,  3,  1,  19, 11,
    10,  90, 15, 17, 10,
    5,  19,  80, 9, 99
  };
  
  int poss[50] = {
    0, 1, 2, 3, 4, 5,
    6, 7, 8, 9, 10, 
    11, 12, 13, 14, 15,
    16, 17, 18, 19, 20
  };
  
  int cost2[] = 
    {
      1, 1, 1, 1, 1, 1, 1
    };
  
  int pos2[]=
    {
      1, 3, 4, 9, 10, 11, 12
    };


  int select[50], seln;  
  dynamic_select_init(50, 5);
  seln = dynamic_select_probe_tokens(costs, 20, select, 1, 3);
  output_sel(costs, 20, select, 1, 3);


  seln = dynamic_select_probe_tokens(costs, 20, select, 2, 3);
  output_sel(costs, 20, select, 2, 3);
  

  seln = dynamic_select_probe_tokens(costs, 20, select, 3, 3);
  output_sel(costs, 20, select, 3, 3);


  seln = dynamic_select_probe_tokens(costs, 20, select, 4, 3);
  output_sel(costs, 20, select, 4, 3);

  seln = dynamic_select_probe_tokens(costs, 20, select, 4, 4);
  output_sel(costs, 20, select, 4, 4);

  fprintf(stdout, "with poss\n");

  seln = dynamic_select_probe_tokens_with_poss(costs, poss, 20, select, 1, 3);
  output_sel(costs, 20, select, 1, 3);


  seln = dynamic_select_probe_tokens_with_poss(costs, poss, 20, select, 2, 3);
  output_sel(costs, 20, select, 2, 3);
  

  seln = dynamic_select_probe_tokens_with_poss(costs, poss, 20, select, 3, 3);
  output_sel(costs, 20, select, 3, 3);


  seln = dynamic_select_probe_tokens_with_poss(costs, poss, 20, select, 4, 3);
  output_sel(costs, 20, select, 4, 3);


  seln = dynamic_select_probe_tokens_with_poss(costs, poss, 20, select, 4, 4);
  output_sel(costs, 20, select, 4, 4);




  seln = dynamic_select_probe_tokens_with_poss(cost2, pos2, 7, select, 2, 3);
  output_sel(cost2, 7, select, 2, 3);


  return 0;
}

