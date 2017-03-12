/* We put the voptimal here  */


typedef struct __v_opt_t{
  int bucks_num;                /* Number of buckets */
  int *buck_total_freq;           /* The total frequency in each bucket. 
                                 This is a array of length bucks_num */
  int *buck_end;                /* The is the end location of each bucket */
} v_opt_t;


/* Build a voptimal histgram from two array of data */
/* Input:                            */
/* bucks_num  is the number of buckets */
/* data_num   is the number of input data */
/* len_list   is a list of data_num number of length */
/* freq_list  is a list of data_num number of freqency */

/* output:    a pointor to a voptimal structure if successfull. NULL if failed  */
v_opt_t * build_voptimal(int bucks_num, int data_num, int *len_list, int *freq_list));



/* Free the allocated memory of v_optimal */
int destroy_voptimal(v_opt_t *v_opt_p);


/* Estimate a frequence between length min and max. (Include the length of min and max) */
/* Return the sum. */
int search_range_voptimal(v_opt_t *v_opt_p, int min, int max);





  





