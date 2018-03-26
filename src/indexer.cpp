#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <hashmap.h>
#include <qgrams.h>
#include <rawdata.h>
#include <indexer.h>
#include <dpselect.h>



index_t * build_index_qgram_super_length(raw_data_t *rp, int tau)
{
  index_t *ip;
  int tkn;
  int prefix_len = rp->raw_q * tau + 1;
  
  
  ip = new index_t;
  ip->token_num = tkn = getHashElemNum(rp->hp);
  
  // init data.
  ip->post_id_list = new int*[tkn];
  ip->post_pos_list = new int*[tkn];
  ip->post_doclen_list = new int*[tkn];
  ip->post_len = new int[tkn];
  
  ip->hists = new basic_hist_t*[tkn];
  ip->underflow_data = 0;
  ip->indexed_tokennum = 0;
  ip->hists_tokennum = 0;  

  // ip->hist_list = new int*[tkn];
  // ip->hist_pos = new int*[tkn];
  // ip->hist_len = new int[tkn];
  
  // Initialize the posting list
  for (int i = HASH_ELEM_START; i < tkn; i++)
  {
    ip->post_len[i] = getFreq(rp->hp, i);
    ip->post_id_list[i] = new int[ip->post_len[i]];
    ip->post_pos_list[i] = new int[ip->post_len[i]];
    ip->post_doclen_list[i] = new int[ip->post_len[i]];
    ip->post_len[i] = 0;
  }
  
  // Go through all the token list to build index.
  for (int i = 0; i < rp->raw_doc_num; i++)
  {
    if (rp->raw_token_num[i] < prefix_len){
      ip->underflow_data ++;
      continue;
    }  
    
    ip->indexed_tokennum += rp->raw_token_num[i];
    
    for (int j = 0; j < rp->raw_token_num[i]; j++)
    {
      int tid = rp->raw_token_list[i][j];
      ip->post_id_list[tid][ip->post_len[tid]] = i;
      ip->post_pos_list[tid][ip->post_len[tid]] = j;
      ip->post_doclen_list[tid][ip->post_len[tid]] = rp->raw_doc_len[i];
      ip->post_len[tid] ++;
    }
  }
  
  
  for (int i = HASH_ELEM_START; i < tkn; i++)
  {
    ip->hists[i] = build_basic_hist(ip->post_len[i], ip->post_doclen_list[i], NULL);
    //ip->hists_tokennum += ip->hists[i]->bucks_num;
    ip->hists_tokennum += (ip->hists[i]->bucks_num < MIN_BUCK_NUM? 0 :ip->hists[i]->bucks_num);
  }
  return ip;
}


index_t * build_index_chunk_super_length(raw_data_t *rp, int tau)
{
  int *freq;
  index_t *ip;
  int tkn = getHashElemNum(rp->hp);
  int minlenpos, maxlenpos;
  minlenpos = maxlenpos = 0;
  int *costs = new int[rp->raw_doc_len[rp->raw_doc_num-1] + 128];
  int *poss = new int[rp->raw_doc_len[rp->raw_doc_num-1] + 128];
  int probel = 0;
  //  int prefix_len = rp->raw_q * tau + 1;

  freq = new int [tkn];
  memset (freq, 0, sizeof(int)*tkn);

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
  
  for (int i = 0; i < rp->raw_doc_num; i++)
  {
    int minlen = rp->raw_token_num[i] - tau;
    int maxlen = rp->raw_token_num[i] + tau;
    while(maxlenpos < rp->raw_doc_num && rp -> raw_token_num[maxlenpos] <= maxlen)
    {
      for (int j = 0; j < rp->raw_token_num[maxlenpos]; j++)
        freq[rp->raw_token_list[maxlenpos][j]]++;
      ++maxlenpos;
    }

    while(minlenpos < rp->raw_doc_num && rp->raw_token_num[minlenpos] < minlen)
    {
      for (int j = 0; j < rp->raw_token_num[minlenpos]; j++)
        freq[rp->raw_token_list[minlenpos][j]]--;
      ++minlenpos;
    }
    
    //    fprintf(stderr, "TOK_COST: %d",i);
    for(int j = 0; j < rp->raw_token_num[i]; j ++)
    {
      costs[j] = freq[rp->raw_token_list[i][j]];
      //      fprintf(stderr, " (%d|%d|%s)",j,costs[j],getKey(rp->hp, rp->raw_token_list[i][j]));
    }
    //fprintf(stderr, "\n");    

    probel = dynamic_select_probe_tokens(costs, rp->raw_token_num[i], poss, tau, rp->raw_q);
    
    rp->sel_tokens[i] = new int[probel];
    rp->sel_pos[i] = new int[probel];
    rp->sel_num[i] = probel;
    //    fprintf(stderr, "TOK_SEL: %d",i);
    if (probel == 0)
    {
      //if (rp->raw_token_num[i] < q * tau + 1){
      ip->underflow_data ++;
      continue;
    }

    for(int j = 0; j < probel; j++){
      rp->sel_pos[i][j] = poss[j];
      rp->sel_tokens[i][j] = rp->raw_token_list[i][rp->sel_pos[i][j]];
      ip->post_len[rp->sel_tokens[i][j]] ++;
      //      fprintf(stderr, " (%d|%d|%s)", rp->sel_tokens[i][j], rp->sel_pos[i][j], getKey(rp->hp, rp->sel_tokens[i][j]));      
    }
    //    fprintf(stderr, "\n");
  }
  
  // ip->hist_list = new int*[tkn];
  // ip->hist_pos = new int*[tkn];
  // ip->hist_len = new int[tkn];
  
  // Initialize the posting list
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
  
  for (int i = HASH_ELEM_START; i < tkn; i++)
  {
    ip->hists[i] = build_basic_hist(ip->post_len[i], ip->post_doclen_list[i], NULL);
    ip->hists_tokennum += (ip->hists[i]->bucks_num < MIN_BUCK_NUM? 0 :ip->hists[i]->bucks_num);
                           //ip->hists_tokennum += ip->hists[i]->bucks_num;
  }
  return ip;
}

index_t * build_index_gram_turbo_length(raw_data_t *rp, int tau)
{
  index_t *ip;
  int tkn = getHashElemNum(rp->hp);
  int probel = 0;
  int prefix_len = rp->raw_q * tau + 1;
  int qgram_probe_len = 2 * rp->raw_q * tau  + 1;
    
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
      if (rp->raw_token_num[i] < qgram_probe_len){
	probel = rp->raw_token_num[i];
      }else{
	probel = prefix_len;
      }
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
  
  for (int i = HASH_ELEM_START; i < tkn; i++)
  {
    ip->hists[i] = build_basic_hist(ip->post_len[i], ip->post_doclen_list[i], NULL);
    ip->hists_tokennum += (ip->hists[i]->bucks_num < MIN_BUCK_NUM? 0 :ip->hists[i]->bucks_num);
  }
  return ip;
}


index_t * build_index_chunk_turbo_length(raw_data_t *rp, int tau)
{
  int *freq;
  index_t *ip;
  int tkn = getHashElemNum(rp->hp);
  int minlenpos, maxlenpos;
  minlenpos = maxlenpos = 0;
  int *costs = new int[rp->raw_doc_len[rp->raw_doc_num-1] + 128];
  int *poss = new int[rp->raw_doc_len[rp->raw_doc_num-1] + 128];
  int *tkpos = new int[rp->raw_doc_len[rp->raw_doc_num-1] + 128];
  int *prefix = new int[rp->raw_doc_len[rp->raw_doc_num-1] + 128];
  int probel = 0;
  int prefix_len = rp->raw_q * tau + 1;

  freq = new int [tkn];
  memset (freq, 0, sizeof(int)*tkn);

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
  
  for (int i = 0; i < rp->raw_doc_num; i++)
  {
    int minlen = rp->raw_token_num[i] - tau;
    int maxlen = rp->raw_token_num[i] + tau;
    while(maxlenpos < rp->raw_doc_num && rp -> raw_token_num[maxlenpos] <= maxlen)
    {
      for (int j = 0; j < prefix_len; j++)
        freq[rp->sorted_token_list[maxlenpos][j]] ++;
      ++maxlenpos;
    }

    while(minlenpos < rp->raw_doc_num && rp->raw_token_num[minlenpos] < minlen)
    {
      for (int j = 0; j < prefix_len; j++)
        freq[rp->sorted_token_list[minlenpos][j]]--;
      ++minlenpos;
    }
    
    if (rp->raw_token_num[i] < prefix_len){
      rp->sel_num[i] = 0;
      ip->underflow_data ++;      
      continue;      
    }
    
        
    int pivotkey = rp->sorted_token_list[i][prefix_len-1];
    int pivotpos = rp->sorted_pos_list[i][prefix_len-1];
    probel = 0;
    for (int j = 0; j < rp->raw_token_num[i]; j++){
      if (rp -> raw_token_list[i][j] < pivotkey || 
          (rp -> raw_token_list[i][j] == pivotkey && j <= pivotpos))
      {
        prefix[probel] = rp->raw_token_list[i][j];
        costs[probel] = freq[prefix[probel]];
        tkpos[probel] = j;
        probel ++;        
      }
    }
    
    // fprintf(stderr, "PREFIX_LIST: %d",i);
    // for(int j = 0; j < prefix_len; j ++)
    // {
    //   fprintf(stderr, " (%d|%d|%d|%s)",j, rp->sorted_token_list[i][j], rp->sorted_pos_list[i][j] ,getKey(rp->hp, rp->sorted_token_list[i][j]));
    // }
    // fprintf(stderr, "\n");
    
    // fprintf(stderr, "TOK_COST: %d",i);
    // for(int j = 0; j < prefix_len; j ++)
    // {
    //   fprintf(stderr, " (%d|%d|%d|%s)",j,costs[j],tkpos[j],getKey(rp->hp, prefix[j]));
    // }
    // fprintf(stderr, "\n");    

    probel = dynamic_select_probe_tokens_with_poss(costs, tkpos,  prefix_len, poss, tau, rp->raw_q);
    
    rp->sel_tokens[i] = new int[probel];
    rp->sel_pos[i] = new int[probel];
    rp->sel_num[i] = probel;
    //fprintf(stderr, "TOK_SEL: %d",i);

    for(int j = 0; j < probel; j++){
      rp->sel_pos[i][j] = tkpos[poss[j]];
      rp->sel_tokens[i][j] = rp->raw_token_list[i][rp->sel_pos[i][j]];
      ip->post_len[rp->sel_tokens[i][j]] ++;
      //fprintf(stderr, " (%d|%d|%s)", rp->sel_tokens[i][j], rp->sel_pos[i][j], getKey(rp->hp, rp->sel_tokens[i][j]));
    }
    //    fprintf(stderr, "\n");
  }
  
  // ip->hist_list = new int*[tkn];
  // ip->hist_pos = new int*[tkn];
  // ip->hist_len = new int[tkn];
  
  // Initialize the posting list
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
  
  for (int i = HASH_ELEM_START; i < tkn; i++)
  {
    ip->hists[i] = build_basic_hist(ip->post_len[i], ip->post_doclen_list[i], NULL);
    //ip->hists_tokennum += ip->hists[i]->bucks_num;
    ip->hists_tokennum += (ip->hists[i]->bucks_num < MIN_BUCK_NUM? 0 :ip->hists[i]->bucks_num);
  }
  return ip;
}








index_t *build_index_chunk_turbine_length(raw_data_t *rp, int tau)
{
  int *freq;
  index_t *ip;
  int tkn = getHashElemNum(rp->hp);
  int minlenpos, maxlenpos;
  minlenpos = maxlenpos = 0;
  int *costs = new int[rp->raw_doc_len[rp->raw_doc_num-1] + 128];
  int *poss = new int[rp->raw_doc_len[rp->raw_doc_num-1] + 128];
  int *tkpos = new int[rp->raw_doc_len[rp->raw_doc_num-1] + 128];
  int *prefix = new int[rp->raw_doc_len[rp->raw_doc_num-1] + 128];
  int probel = 0;
  int prefix_len = rp->raw_q * tau + 1;

  freq = new int [tkn];
  memset (freq, 0, sizeof(int)*tkn);

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
  
  for (int i = 0; i < rp->raw_doc_num; i++)
  {
    int minlen = rp->raw_token_num[i] - tau;
    //int maxlen = rp->raw_token_num[i] + tau;

    while(minlenpos < rp->raw_doc_num && rp->raw_token_num[minlenpos] < minlen){
      for (int j = 0; j < probel; j++)
        freq[rp->sel_tokens[minlenpos][j]]--;
      ++minlenpos;
    }
    
    if (rp->raw_token_num[i] < prefix_len){
      rp->sel_num[i] = 0;
      ip->underflow_data ++;      
      continue;      
    }
        
    int pivotkey = rp->sorted_token_list[i][prefix_len-1];
    int pivotpos = rp->sorted_pos_list[i][prefix_len-1];

    probel = 0;
    for (int j = 0; j < rp->raw_token_num[i]; j++){
      if (rp -> raw_token_list[i][j] < pivotkey || 
          (rp -> raw_token_list[i][j] == pivotkey && j <= pivotpos))
      {
        prefix[probel] = rp->raw_token_list[i][j];
        costs[probel] = freq[prefix[probel]];
        tkpos[probel] = j;
        probel ++;        
      }
    }
    
    probel = dynamic_select_probe_tokens_with_poss(costs, tkpos,  prefix_len, poss, tau, rp->raw_q);
    
    rp->sel_tokens[i] = new int[probel];
    rp->sel_pos[i] = new int[probel];
    rp->sel_num[i] = probel;
    //fprintf(stderr, "TOK_SEL: %d",i);

    for(int j = 0; j < probel; j++){
      rp->sel_pos[i][j] = tkpos[poss[j]];
      rp->sel_tokens[i][j] = rp->raw_token_list[i][rp->sel_pos[i][j]];
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
  
  for (int i = HASH_ELEM_START; i < tkn; i++)
  {
    ip->hists[i] = build_basic_hist(ip->post_len[i], ip->post_doclen_list[i], NULL);
    //ip->hists_tokennum += ip->hists[i]->bucks_num;
    ip->hists_tokennum += (ip->hists[i]->bucks_num < MIN_BUCK_NUM? 0 :ip->hists[i]->bucks_num);
  }
  return ip;
}






void dump_index(FILE *fp, index_t *ip, raw_data_t *rp)
{
  int tkn;
  tkn = ip->token_num;
  
  
  for(int i = 1; i < tkn; i++)
  {
    if (ip->post_len[i] <= 0) continue;
    
    fprintf(fp, "\nTOK id=%d str=\"%s\" --> ", i, getKey(rp->hp, i));
    //    fprintf(fp, "HISTLOG: %d %d\" --> ", i, ip->post_len[i]);

    for (int j = 0; j < ip->post_len[i]; j++)
      fprintf(fp, "(%d|%d|%d) ", ip->post_id_list[i][j],
              ip->post_pos_list[i][j], ip->post_doclen_list[i][j]);

    fprintf(fp,"\nHIST: ");
    
    for (int j = 0; j < ip->hists[i]->bucks_num; j++)
      fprintf(fp, "(%d|%d|%d) ", ip->hists[i]->buck_keys[j], ip->hists[i]->buck_pos[j], ip->hists[i]->buck_freq[j]);

    fprintf(fp,"\n");
  }

  // for(int i = 1; i < tkn; i++){    
  //   if (ip->post_len[i] <= 0) continue;
  //   fprintf(fp, "HISTLOG: ID %d %d %d\n", i, ip->post_len[i], ip->hists[i]->bucks_num);
  //   fprintf(fp, "HISTLOG: LEN ");
  //   for (int j = 0; j < ip->hists[i]->bucks_num; j++)
  //     fprintf(fp, "%d ", ip->hists[i]->buck_keys[j]);
  //   fprintf(fp, "\n");
  //   fprintf(fp, "HISTLOG: FREQ ");
  //   for (int j = 0; j < ip->hists[i]->bucks_num; j++)
  //     fprintf(fp, "%d ", ip->hists[i]->buck_freq[j]);
  //   fprintf(fp, "\n");
  // }
}
