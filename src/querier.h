#ifndef __QUERIER_H__
#define __QUERIER_H__

#include "bitset.h"
#include "rawdata.h"
#include "indexer.h"
#include "qgrams.h"


//#define MAX_QUERY_LEN 1024*1024
#define LEN_OVERHEAD 128

typedef struct __query_t{
  int q;                        /* The length of qgram */
  int max_len;                  /* maximal query length */
  int qry_len;                  /* Current query length */
  char *qry_str;                /* Current query String */
  int *token_id_list;           /* Token list for Current query */
  char **token_str_list;        /* Token string list for current query */
  int *token_costs;              /* Token cost for this query  */
  int *token_start;             /* Token start positon  */
  int *token_end ;              /* Token end positon  */
  int token_num;                /* Token number */
  int *prefix_tokens;          /* Prefix tokens */
  int *prefix_pos;             /* Prefix token position */
  int *prefix_costs;            /* Prefix costs  */
  int *prefix_start;           /* Prefix start */
  int *prefix_end;             /* prefix end */
  int prefix_num ;             /* Number of prefix */
  int *probe_tokens;            /* Probe token list */
  int *probe_pos;               /* Probe token position */
  int probe_num;                /* Probe token number */
  int tau;                      /* Threshold for current query */
  int *cand_list;               /* The candidate list */
  int cand_num;                 /* The candidate number */
  bit_map_t *candset;           /* Candidate set */
#ifdef VCN_FILTER_LEN
  int vcn[VCN_FILTER_LEN];
#endif 
}query_t;



int indexGramSuperQueryPreprocessing(char *qry, int qrylen, raw_data_t *rp, index_t *ip, query_t *qp, int tau);

int indexChunkSuperQueryPreprocessing(char *qry, int qrylen, raw_data_t *rp, index_t *ip, query_t *qp, int tau);

int indexGramTurboQueryPreprocessing(char *qry, int qrylen, raw_data_t *rp, index_t *ip, query_t *qp, int tau);

int indexChunkTurboQueryPreprocessing(char *qry, int qrylen, raw_data_t *rp, index_t *ip, query_t *qp, int tau);



/* Create a query data structure. Use the data structure */
query_t *init_query(raw_data_t *rp, int tau);


/* We are not so sure. */
int dynamic_select_probe_tokens(query_t *qp, int tau);


/* Set a query data structure  */
int setup_query(query_t *qp, char *qry_str, int qry_len, raw_data_t *rp, int tau);

int setup_query_krhash(query_t *qp, char *qry_str, int qry_len, raw_data_t *rp, int tau);

/* Set a query data structure  */
int cal_probe_cost(query_t *qp, index_t *ip);


int cal_prefix_cost(query_t *qp, index_t *ip, int tau, int q);
int cal_prefix(query_t *qp, index_t *ip, int tau, int q);

#endif








