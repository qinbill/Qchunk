#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <hashmap.h>
#include <qgrams.h>
#include <rawdata.h>
#include <indexer.h>
#include <dpselect.h>


index_t * build_qgram_index(raw_data_t *rp, int tau)
{
  index_t *ip;
  int tkn = getHashElemNum(rp->hp);
  int probel = 0;
  int prefix_len = rp->raw_q * tau + 1;
  int qgram_probe_len = 2 * rp->raw_q * tau  + 1;

  // Need to calculate qgram length here. 
    
  ip = new index_t;
  ip->token_num = tkn;
  
  // init data.
  ip->post_id_list = new int*[tkn];
  ip->post_pos_list = new int*[tkn];
  ip->post_doclen_list = new int*[tkn];
  ip->post_len = new int[tkn];

  ip->hists = new basic_hist_t*[tkn];
  ip->underflow_data = 0;  
  ip->indexed_tokennum = 0;
  ip->hists_tokennum = 0;

  rp->sel_tokens = new int*[rp->raw_doc_num];
  rp->sel_pos = new int*[rp->raw_doc_num];
  rp->sel_num = new int[rp->raw_doc_num];
  
  
  for (int i = 0; i < rp->raw_doc_num; i++){
    
    rp->sel_tokens[i] = rp->sorted_token_list[i];
    rp->sel_pos[i] = rp->sorted_pos_list[i];
    
    if (rp->raw_token_num[i] < prefix_len){
      probel = 0; 
      ip->underflow_data ++;
    }else{
      probel = prefix_len;
    }

    rp->sel_num[i] = probel;

    for(int j = 0; j < probel; j++){
      ip->post_len[rp->sel_tokens[i][j]] ++;
    }
  }

  for (int i = HASH_ELEM_START; i < tkn; i++)
  {
    //ip->post_len[i] = getFreq(rp->hp, i);
    if(ip->post_len[i]>0)
    {
      ip->post_id_list[i] = new int[ip->post_len[i]];
      ip->post_pos_list[i] = new int[ip->post_len[i]];
      ip->post_doclen_list[i] = new int[ip->post_len[i]];
      ip->post_len[i] = 0;
    }
  }
  
  // Go through all the token list to build index.
  for (int i = 0; i < rp->raw_doc_num; i++)
  {
    ip->indexed_tokennum += rp->sel_num[i];
    for (int j = 0; j < rp->sel_num[i]; j++)
    {
      int tid = rp->sel_tokens[i][j];
      if (tid <=0) continue;
      int tid_pos = rp->sel_pos[i][j];
      ip->post_id_list[tid][ip->post_len[tid]] = i;
      ip->post_pos_list[tid][ip->post_len[tid]] = tid_pos;
      ip->post_doclen_list[tid][ip->post_len[tid]] = rp->raw_doc_len[i];
      ip->post_len[tid]++;
    }
  }
  
  return ip;
}
