#include <stdlib.h>
#include <string.h>
#include "rawdata.h"
#include "qgrams.h"
#include "querier.h"
#include "indexer.h"
#include "sorting.h"
#include "dpselect.h"



extern int underflowNum;

int indexGramSuperQueryPreprocessing(char *qry, int qrylen, raw_data_t *rp, index_t *ip, query_t *qp, int tau) {
  int i, ret;
  if (setup_query_krhash(qp, qry, qrylen, rp, tau) == 0){
    underflowNum++;
    return -1;
  }
  
  cal_probe_cost(qp, ip);
  qp->probe_num = dynamic_select_probe_tokens(qp->token_costs, qp->token_num, qp->probe_pos, tau, rp->raw_q);
  for(i = 0; i <= tau + 1; i++) 
    qp->probe_tokens[i] = qp->token_id_list[qp->probe_pos[i]];

  ret = qp->probe_num;
  return ret;
}


int indexChunkSuperQueryPreprocessing(char *qry, int qrylen, raw_data_t *rp, index_t *ip, query_t *qp, int tau){
  int ret;
  
  if (setup_query_krhash(qp, qry, qrylen, rp, tau) == 0){
    underflowNum++;
    return -1;
  }

  cal_probe_cost(qp, ip);
  qp->probe_num = qp -> token_num;
  ret =  qp->probe_num;
  return ret;
}



int indexGramTurboQueryPreprocessing(char *qry, int qrylen, raw_data_t *rp, index_t *ip, query_t *qp, int tau)
{
  int ret;
  if (setup_query_krhash(qp, qry, qrylen, rp, tau) == 0){
    underflowNum++;
    return -1;
  }
  cal_prefix_cost(qp, ip, tau, rp->raw_q);
  //cal_probe_cost(qp, ip);
  ret = qp->probe_num;
  return ret;
}



int indexChunkTurboQueryPreprocessing(char *qry, int qrylen, raw_data_t *rp, index_t *ip, query_t *qp, int tau)
{
  int ret;
  if (setup_query_krhash(qp, qry, qrylen, rp, tau) == 0){
    underflowNum++;
    return -1;
  }
  cal_prefix(qp, ip, tau, rp->raw_q);
  ret = qp->probe_num;
  
  return ret;
}




/* Create a query data structure. Use the data structure */
query_t *init_query(raw_data_t *rp, int tau)
{
  query_t *qp = new query_t;
  qp -> max_len = rp->raw_doc_len[rp->raw_doc_num-1] + 128;
  qp -> qry_str = NULL;
  qp -> qry_len = 0;
  qp -> q = rp->raw_q;
  qp -> token_str_list = new char *[qp -> max_len];
  for (int i = 0; i < qp -> max_len; i++)
    qp -> token_str_list[i] = new char[rp -> raw_q + 1];  
  qp -> token_id_list = new int[qp -> max_len];
  qp -> token_costs = new int[qp -> max_len];
  qp -> token_start = new int[qp -> max_len];
  qp -> token_end = new int[qp -> max_len];
  qp -> token_num  = 0;

  qp -> prefix_tokens = new int[qp -> max_len];
  qp -> prefix_pos = new int[qp -> max_len];
  qp -> prefix_costs = new int[qp -> max_len];
  qp -> prefix_start = new int[qp -> max_len];
  qp -> prefix_end = new int[qp -> max_len];

  
  qp -> probe_tokens = new int[qp -> max_len];
  qp -> probe_pos  = new int[qp -> max_len];
  qp -> probe_num = 0;
  qp -> tau = tau;
  qp -> cand_list = new int[rp->raw_doc_num];
  qp -> candset  = new bit_map_t;
  bit_map_init(qp -> candset, rp->raw_doc_num);

  return qp;
}

int setup_query(query_t *qp, char *qry_str, int qry_len, raw_data_t *rp, int tau)
{
  int tkn = 0;    
  qp -> qry_str = qry_str;
  qp -> qry_len = qry_len;
  
  tkn = doc2QgramPatch (qp->token_str_list, qry_str, qry_len, rp->raw_q);
  
  /* Token number */
  qp->token_num = tkn;


  if(tkn < rp->raw_q * tau + 1)
    return 0;

#ifdef VCN_FILTER_LEN
  calculate_vcn_vector(qry_str, qry_len, qp->vcn);
#endif

  // Now we need to transfer qglist into token idlist. 
  for (int i = 0; i < tkn; i++){
    qp -> token_id_list[i] = searchKey(rp->hp, qp->token_str_list[i]);
    /* search in the list */
    //qp -> token_cost[i] = getFreq(rp->hp, qp -> token_id_list[i]);
  }  
  return tkn;
}


int setup_query_krhash(query_t *qp, char *qry_str, int qry_len, raw_data_t *rp, int tau)
{
  int tkn = 0;
  qp -> qry_str = qry_str;
  qp -> qry_len = qry_len;
  
  //fprintf(stderr, "ORIG %s\n", qry_str);

  memset (qry_str + qry_len, SUFFIX_PATCH_CHAR, rp->raw_q);
  tkn = qry_len + rp->raw_q + rp->raw_q - 2;
  //  tkn = DOC_TOK_NUM(qry_len, rp->raw_q);
  qry_str[qry_len + rp->raw_q - 1] = '\0';
  //fprintf(stderr, "PATCHED %s\n", qry_str - rp->raw_q + 1);
  //  tkn = doc2QgramPatch (qp->token_str_list, qry_str, qry_len, rp->raw_q);
  tkn = rkRollingStringSearch(rp->hp, qry_str - rp->raw_q + 1, tkn, qp->token_id_list);
  
  if(tkn < rp->raw_q * tau + 1)
    return 0;
    
#ifdef VCN_FILTER_LEN
  calculate_vcn_vector(qry_str, qry_len, qp->vcn);
#endif

  /* Token number */
  qp->token_num = tkn;
  qry_str[qry_len] = '\0';
  if(tkn < rp->raw_q * tau + 1)
    return 0;
  return tkn;
}

int cal_probe_cost(query_t *qp, index_t *ip)
{
  int keymin = qp->qry_len - qp->tau;
  int keymax = qp->qry_len + qp->tau;
  int start_pos;
  int end_pos;
  
  for (int i = 0; i < qp->token_num; i++)
  {
    int tid = qp->token_id_list[i];
    if (tid > 0)
    {
      qp->token_costs[i] = search_range_basic_hist(ip->hists[tid], keymin, keymax,
                                                  &start_pos, &end_pos);
      qp->token_start[i] = start_pos;
      qp->token_end[i] = end_pos;
    }else
    {
      qp->token_costs[i] = 0;
      qp->token_start[i] = 0;
      qp->token_end[i] = 0;
    }    
  }
  return 0;
}

int cal_prefix_cost(query_t *qp, index_t *ip, int tau, int q)
{
  int tkn = qp->token_num;
  int keymin = qp->qry_len - qp->tau;
  int keymax = qp->qry_len + qp->tau;
  int start_pos;
  int end_pos;
  int k = q * tau + 1;
  


  for (int i = 0; i < tkn; i ++ ){
    qp->prefix_tokens[i] = qp->token_id_list[i];
    qp->prefix_pos[i] = i;
  }
  quickSelectKTwoKeys(qp->prefix_tokens, qp->prefix_pos, 0, tkn - 1, k - 1);
  
  int pivotid = qp->prefix_tokens[k - 1];
  int pivotpos = qp->prefix_pos[k - 1];
  qp->prefix_num = 0;
  for (int i = 0; i < tkn; i ++ ){
    if ( qp->token_id_list[i] < pivotid ||
         (qp->token_id_list[i] == pivotid && i <= pivotpos))
    {
      qp->prefix_tokens[qp->prefix_num] = qp->token_id_list[i];
      qp->prefix_pos[qp->prefix_num] = i;
      qp->prefix_num++;
    }
  }
  
  for (int i = 0; i < qp->prefix_num; i++)
  {
    int tid = qp->prefix_tokens[i];
    //int pos = qp->prefix_tokens[pos];
    if (tid > 0)
    {
      qp->prefix_costs[i] = search_range_basic_hist(ip->hists[tid], keymin, keymax,
                                                   &start_pos, &end_pos);
      qp->prefix_start[i] = start_pos;
      qp->prefix_end[i] = end_pos;
    }else{
      qp->prefix_costs[i] = 0;
      qp->prefix_start[i] = 0;
      qp->prefix_end[i] = 0;
    }
  }
  
  int probel = dynamic_select_probe_tokens_with_poss(qp->prefix_costs, qp->prefix_pos, qp->prefix_num,
                                        qp->probe_pos, tau, q);
  
  for (int i = 0; i < probel; i++){
    int pfxpos = qp->probe_pos[i];
    int relpos = qp->prefix_pos[pfxpos];
    qp->probe_pos[i] = relpos;
    qp->probe_tokens[i] = qp->token_id_list[relpos];
    qp->token_start[relpos] = qp->prefix_start[pfxpos];
    qp->token_end[relpos] = qp->prefix_end[pfxpos];    
    qp->token_costs[relpos] = qp->prefix_costs[pfxpos];
  }
  qp->probe_num = probel;

  return probel;  
}





int cal_prefix(query_t *qp, index_t *ip, int tau, int q)
{
  int tkn = qp->token_num;
  int keymin = qp->qry_len - qp->tau;
  int keymax = qp->qry_len + qp->tau;
  int start_pos;
  int end_pos;
  int probel;  
  int k = q*tau + 1;

  for (int i = 0; i < tkn; i ++ ){
    qp->prefix_tokens[i] = qp->token_id_list[i];
    qp->prefix_pos[i] = i;
  }
  quickSelectKTwoKeys(qp->prefix_tokens, qp->prefix_pos, 0, tkn - 1, k - 1);
  
  int pivotid = qp->prefix_tokens[k - 1];
  int pivotpos = qp->prefix_pos[k - 1];
  qp->prefix_num = 0;
  for (int i = 0; i < tkn; i ++ ){
    if ( qp->token_id_list[i] < pivotid ||
         (qp->token_id_list[i] == pivotid && i <= pivotpos))
    {
      qp->prefix_tokens[qp->prefix_num] = qp->token_id_list[i];
      qp->prefix_pos[qp->prefix_num] = i;
      qp->prefix_num++;
    }
  }
  


  for (int i = 0; i < qp->prefix_num; i++)
  {
    int tid = qp->prefix_tokens[i];
    //int pos = qp->prefix_tokens[pos];
    if (tid > 0)
    {
      qp->prefix_costs[i] = search_range_basic_hist(ip->hists[tid], keymin, keymax,
                                                   &start_pos, &end_pos);
      qp->prefix_start[i] = start_pos;
      qp->prefix_end[i] = end_pos;
    }else{
      qp->prefix_costs[i] = 0;
      qp->prefix_start[i] = 0;
      qp->prefix_end[i] = 0;
    }
  }

  // fprintf(stderr, "PREFIX_LIT: ");
  // for(int j = 0; j < k;  j ++)
  // {
  //   fprintf(stderr, " (%d|%d|%d|%d)",j,qp->prefix_tokens[j], qp->prefix_costs[j], qp->prefix_pos[j]);
  // }
  // fprintf(stderr, "\n");    
  


  qp->probe_num = probel = qp->prefix_num;

  for (int i = 0; i < probel; i++){
    int relpos = qp->prefix_pos[i];
    qp->probe_pos[i] = relpos;
    qp->probe_tokens[i] = qp->token_id_list[relpos];
    qp->token_start[relpos] = qp->prefix_start[i];
    qp->token_end[relpos] = qp->prefix_end[i];    
    qp->token_costs[relpos] = qp->prefix_costs[i];
  }

  return probel;  
}
