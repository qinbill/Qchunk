#ifndef __INDEXER_H__
#define __INDEXER_H__

#include <math.h>
#include "basichist.h"
#include "rawdata.h"


typedef struct __index_t
{
  int token_num;  
  int **post_id_list;
  int **post_doclen_list;
  int **post_pos_list;
  int *post_len;
  basic_hist_t **hists;
  int underflow_data;
  int indexed_tokennum;
  int hists_tokennum;

} index_t;




index_t * build_index_chunk_super_length(raw_data_t *rp, int tau);

index_t * build_index_qgram_super_length(raw_data_t *rp, int tau);

index_t * build_index_gram_turbo_length(raw_data_t *rp, int tau);

index_t * build_index_chunk_turbo_length(raw_data_t *rp, int tau);

index_t * build_index_chunk_turbine_length(raw_data_t *rp, int tau);

void dump_index(FILE *fp, index_t *ip, raw_data_t *rp);

#endif
