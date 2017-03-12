#ifndef __BASICHIST_H__
#define __BASICHIST_H__




#define MIN_BUCK_NUM 5
/* We put the voptimal here  */

typedef struct _basic_hist_t
{
  int bucks_num;
  int *buck_keys;
  int *buck_freq;
  int *buck_pos;
} basic_hist_t;



// Now, we need to build a histgram list
// We use a basic one right now to minimize.
///* This one takes a length list and freq list and build a histgram. */
basic_hist_t *build_basic_hist(int data_num, int *len_list, int *freq_list);


void destroy_basic_hist(basic_hist_t *bhp);


/* Calculetefrequence between length min and max. 
   (Include the length of min and max) */
/* Return the sum. */
int search_range_basic_hist(basic_hist_t *bhp, int min, int max, int *start_pos, int *end_pos);


#endif
  





