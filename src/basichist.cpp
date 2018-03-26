#include <stdlib.h>
#include <string.h>
#include "header.h"
#include "indexer.h"
#include "basichist.h"


///* This one takes a length list and freq list and build a histgram. */
basic_hist_t *build_basic_hist(int data_num, int *len_list, int *freq_list){
  basic_hist_t *bhp = new basic_hist_t;
  
  // Now, we need to build a histgram list
  // We use a basic one right now to minimize.
  int tmp_len_hist[MAX_DOC_LEN];
  int tmp_freq_hist[MAX_DOC_LEN];
  int tmp_pos_hist[MAX_DOC_LEN];
  int tmp_pos = -1;
  
  tmp_len_hist[0] = -1;
  for (int i = 0; i < data_num; i++)
  {
    int len = len_list[i];    
    if (tmp_pos < 0 || (tmp_pos >= 0 && tmp_len_hist[tmp_pos] != len)){
      tmp_pos ++;
      tmp_len_hist[tmp_pos] = len;
      tmp_freq_hist[tmp_pos] = 1;
      tmp_pos_hist[tmp_pos] = i;      
    }else{
      tmp_freq_hist[tmp_pos] ++;
    }
  }
  bhp->buck_keys = new int[tmp_pos+2];
  bhp->buck_pos  = new int[tmp_pos+2];
  bhp->buck_freq = new int[tmp_pos+2];
  bhp->bucks_num = tmp_pos + 1;
  
  memcpy(bhp->buck_keys, tmp_len_hist, sizeof(int) * bhp->bucks_num);
  memcpy(bhp->buck_pos,  tmp_pos_hist, sizeof(int) * bhp->bucks_num);
  memcpy(bhp->buck_freq, tmp_freq_hist, sizeof(int) * bhp->bucks_num);
  bhp->buck_pos[bhp->bucks_num] = data_num;
  bhp->buck_freq[bhp->bucks_num] = 0;
  bhp->buck_keys[bhp->bucks_num] = INF;
  
  return bhp;  
}


void destroy_basic_hist(basic_hist_t *bhp)
{
  delete [] bhp->buck_keys;
  delete [] bhp->buck_pos;
  delete [] bhp->buck_freq;
  delete bhp;
}



/* We put the voptimal here  */

// Find the first in the list which is no less than key.
static inline int bsearch_nlt ( int *key_list, int l, int r, int key ){
  int mid, left = l, right = r;
  int ret = right;
  
  while ( left < right ){
    mid = ( left + right ) >> 1;
    if ( key > key_list[mid] ){
      left = mid + 1;
    }else if ( key < key_list[mid] ){
      ret = mid;
      right = mid;
    }else{
      ret = mid;
      break;
    }
  }
  //while ( ret > l && key_list[ret-1] >= key) ret --;
  return ret;
}

// // Find the first in the list which is larger than key.
// static inline int bsearch_ngt ( int *key_list, int l, int r, int key ){
//   int mid, left = l, right = r;
//   int ret = right;
  
//   while ( left < right ){
//     mid = ( left + right ) >> 1;    
//     if ( key > key_list[mid] ){
//       left = mid + 1;
//       ret = mid + 1;
//     }else if ( key < key_list[mid] ){
//       right = mid;
//     }else{
//       ret = mid;
//       break;
//     }
//   }
//   //  while ( ret < r && key_list[ret] <= key ) ret ++;
//   //  while ( ret + 1 < r && slen[list[ret+1].str] <= key) ret ++;
//   return ret;
// }


/* Estimate a frequence between length min and max. (Include the length of min and max) */
/* Return the sum. */
int search_range_basic_hist(basic_hist_t *bhp, int min, int max, int *start_pos, int *end_pos)
{
  int start_id, end_id;
  
  start_id = bsearch_nlt(bhp->buck_keys, 0, bhp->bucks_num, min);
  end_id   = bsearch_nlt(bhp->buck_keys, start_id, bhp->bucks_num, max+1);

  *start_pos = bhp->buck_pos[start_id];
  *end_pos = bhp->buck_pos[end_id];

  return (*end_pos) - (*start_pos);
}



