#include "bitset.h"
#include "prober.h"
#include "bitset.h"
#include "verify.h"





// This part probe the invertd index and generate the candidates.

// Probe the index.
int probing_qgrams_length(raw_data_t *rp, query_t *qp, index_t *ip)
{
  int probel = qp->probe_num;
  int tau = qp->tau;
  int qlen = qp->qry_len;
  
  //return 0;
  
  qp -> cand_num = 0;  
  bit_map_clear(qp->candset);
  
  for (int i = 0; i < probel; i ++ )
  {
    int tok = qp -> probe_tokens[i];
    int pos = qp -> probe_pos[i];
    int cost = qp -> token_costs[pos];
    if (tok == 0 || cost==0) continue;
    int start = qp -> token_start[pos];
    int end = qp -> token_end[pos];
    for (int j = start; j < end; j++)
    {
      int dpos = ip->post_pos_list[tok][j];
      int ddoc = ip->post_id_list[tok][j];
      int dlen = rp->raw_doc_len[ddoc];
      
      if (ABS(dpos - pos) + ABS((qlen-pos)-(dlen-dpos)) <= tau)
      {
        // Check if it is a candaidate already
        if (bit_map_check(qp->candset, ddoc)!=0) continue;        
#ifdef VCN_FILTER_LEN
        if (vcn_filter(qp->vcn, rp->vcns[ddoc], tau)==0){
          //printf("FIltered by vcn\n");
          continue;
        }        
#endif
        bit_map_set(qp->candset, ddoc);
        qp->cand_list[qp->cand_num++] = ddoc;        
      }
    }
  }
  return qp->cand_num;
}


// We use this method to do furter filtering and 
// Verification.
int filter_verify(raw_data_t *rp, query_t *qp)
{
  int res = 0;
  // Without add any proper verfication. Just verfiy it and output.
  for (int i = 0; i < qp->cand_num; i++) {
    int y = qp->cand_list[i];
    int ed = edit_distance(qp->qry_len, qp->qry_str, rp->raw_doc_len[y],
                           rp->raw_doc_str[y], qp->tau);
    if (ed <= qp->tau) {
      fprintf(stdout, "RESULT: %d %d %d %d\n%s\n%s\n\n", ed, y, qp->qry_len, rp->raw_doc_len[y], qp->qry_str, rp->raw_doc_str[y]);
      res ++;
#ifdef REAL_PROF
      ed = edit_distance_real(qp->qry_len, qp->qry_str, rp->raw_doc_len[y],
                           rp->raw_doc_str[y], qp->tau);
#endif
    }
  }
  return res;  
}




