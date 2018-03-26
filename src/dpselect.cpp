#include <stdlib.h>
#include <string.h>
#include "rawdata.h"
#include "qgrams.h"
#include "querier.h"
#include "indexer.h"


// This part is now multithreshold safe.
static uint **dyn_table = NULL;
static uint **dyn_path = NULL;
static int dyn_size;

int dynamic_select_init(int toknum, int tau)
{
  if (dyn_table == NULL){
    dyn_size = toknum;
    dyn_table = new uint*[tau + 2];
    dyn_path = new uint*[tau + 2];
    for (int i = 0; i < tau + 2; i++){
      dyn_table[i] = new uint [dyn_size];
      dyn_path[i] = new uint [dyn_size];
    }
  }
  return 0;
}

// Select the tokens for probing.
int dynamic_select_probe_tokens(int *costs, int costlen, 
                                int *sel_pos, int tau, int q)
{
  int tkn = costlen;
  uint inf;;
  //  int min;  

  if ( costlen < q * tau + 1)
    return 0;                   // Underflow.
  
  /* Setup memory for dyn table */
  for (int i = 1; i <= tau + 1; i ++)
  {    
    memset(dyn_table[i], 128,  sizeof(int) * tkn);
  }
  inf = dyn_table[1][0];

  dyn_table[1][0] = costs[0];
  for (int i = 1; i < tkn; i ++)
  {
    if (dyn_table[1][i-1] <= (uint)costs[i])
    {
      dyn_table[1][i] = dyn_table[1][i-1];
      dyn_path[1][i] = dyn_path[1][i-1];
    }else
    {
      dyn_table[1][i] = costs[i];
      dyn_path[1][i] = i;
    }
  }
  
  for (int i = q; i < tkn; i ++)
  {
    for (int j = 2; j <= tau + 1; j++)
    {
      if (dyn_table[j][i-1] <= dyn_table[j-1][i-q] + costs[i])
      {        
        dyn_table[j][i] = dyn_table[j][i-1];        
        dyn_path[j][i] = dyn_path[j][i-1];
      }else{
        dyn_table[j][i] = dyn_table[j-1][i-q] + costs[i];
        dyn_path[j][i] = i;
      }
      if (dyn_table[j][i] >= inf)
        break;
    }
  }  

  /* Now let's select it */
  int ppath = dyn_path[tau+1][tkn-1];
  for (int i = tau + 1; i >=1; i --)
  {
    //    sel[i-1] = qp->token_id_list[ppath];
    sel_pos[i-1] = ppath;
    ppath = dyn_path[i-1][ppath-q];
  }

// #ifdef DEBUG
//   fprintf(stderr, "\n Cost list-> ");
//   for (int i = 0; i < tkn; i++)
//   {
//     fprintf(stderr, "[%d|%d|%d] ", i,  qp -> token_id_list[i], qp->token_cost[i]);
//   }
//   fprintf(stderr, "\n DYN Table-> \n");
//   for (int i = 1; i <= tau + 1; i ++)
//   {
//     for (int j = 0; j < tkn; j++)
//     {
//       fprintf(stderr, "%d\t", dyn_table[i][j]>=inf? -1: dyn_table[i][j]);
//     }
//     fprintf(stderr, "\n");
//   }
//   fprintf(stderr, "\n Selected-> \n");
//   for (int i = 0; i < tau + 1; i ++)
//   {
//     fprintf(stderr, "[%d|%d] ", qp->probe_tokens[i], qp->probe_pos[i]);
//   }

//   fprintf(stderr, "\n");
// #endif

  return tau + 1;
}


int dynamic_select_probe_tokens_with_poss(int *costs, int *poss, int costlen, 
                                int *sel_pos, int tau, int q)
{
  int tkn = costlen;
  uint inf;
  long long gaps[1024*2];
  //  int min;
  gaps[0] = -1;

  /* Setup memory for dyn table */
  for (int i = 1; i <= tau + 1; i ++){
    memset(dyn_table[i], 128,  sizeof(int) * tkn);
  }
  inf = dyn_table[1][0];

  dyn_table[1][0] = costs[0];
  for (int i = 1; i < tkn; i ++)
  {
    if (dyn_table[1][i-1] <= (uint)costs[i])
    {
      dyn_table[1][i] = dyn_table[1][i-1];
      dyn_path[1][i] = dyn_path[1][i-1];
    }else{
      dyn_table[1][i] = costs[i];
      dyn_path[1][i] = i;
    }
  }

  // Need to fix this part.
  for (int i = 1; i < tkn; i ++){
    int k = i - 1;
    while ( k>=0 && poss[i] - poss[k] < q)
      k--;
    gaps[i] = i - k;
    if(k < 0) continue;
    for (int j = 2; j <= tau + 1; j++){
      if (dyn_table[j][i-1] <= dyn_table[j-1][k] + costs[i]){
        dyn_table[j][i] = dyn_table[j][i-1];
        dyn_path[j][i] = dyn_path[j][i-1];
      }else{
        dyn_table[j][i] = dyn_table[j-1][k] + costs[i];
        dyn_path[j][i] = i;
      }
      if (dyn_table[j][i] >= inf)
        break;
    }
  }

  if (dyn_table[tau+1][tkn-1] >= inf)
    return 0;                   // Underflow

  /* Now let's select it */
  long long ppath = dyn_path[tau+1][tkn-1];
  for (int i = tau + 1; i >=1; i --)
  {
    sel_pos[i-1] = ppath;
    ppath = dyn_path[i-1][ppath-gaps[ppath]];
  }

// //#ifdef DEBUG
//   fprintf(stderr, "\n Cost list-> ");
//   for (int i = 0; i < tkn; i++)
//   {
//     fprintf(stderr, "[%d|%d|%d] ", i,  costs[i], poss[i]);
//   }
//   fprintf(stderr, "\n DYN Table-> \n");
//   for (int i = 1; i <= tau + 1; i ++)
//   {
//     for (int j = 0; j < tkn; j++)
//     {
//       fprintf(stderr, "%d\t", dyn_table[i][j]>=inf? -1: dyn_table[i][j]);
//     }
//     fprintf(stderr, "\n");
//   }
//   fprintf(stderr, "\n Selected-> \n");
//   for (int i = 0; i < tau + 1; i ++)
//   {
//     fprintf(stderr, "[%d|%d] ", costs[sel_pos[i]], sel_pos[i]);
//   }
//   fprintf(stderr, "\n");
// //#endif

  return tau + 1;
}


