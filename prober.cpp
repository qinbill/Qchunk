#include "bitset.h"
#include "prober.h"
#include "bitset.h"
#include "verify.h"

int prefix_filtering_probe(raw_data_t *rp, query_t *qp, index_t *ip)
{
  int probel = qp->probe_num;
  int tau = qp->tau;
  int qlen = qp->qry_len;
  
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
        bit_map_set(qp->candset, ddoc);
        qp->cand_list[qp->cand_num++] = ddoc;        
      }
    }
  }
  return qp->cand_num;
}
