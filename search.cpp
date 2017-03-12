#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <stdlib.h>
#include <iomanip>
#include <string.h>
#include <getopt.h>
#include "qgrams.h"
#include "hashmap.h"
#include "rawdata.h"
#include "indexer.h"
#include "querier.h"
#include "prober.h"
#include "header.h"
#include "dpselect.h"
#include "usage.h"
#include "sorting.h"
#include "verify.h"


char g_version[]=VERSION;

using namespace std;


int candNum = 0;
int resultNum = 0;
int queryNum = 0;
int underflowNum = 0;
int useposition = 0;


void print_version(){
  fprintf(stderr, "Version: %s\n", g_version);
}

void print_usage(){
  fprintf(stderr, "usage: -t <Max Edit Distance>    :Edit distance threshold for index building.>\n");
  fprintf(stderr, "       -i <input file name>      :input binary file prefix\n");
  fprintf(stderr, "       -p                        :use positional filtering\n");
  print_version();
  exit(0);
}



index_t * build_gram_index(raw_data_t *rp, int tau)
{
  index_t *ip;
  int tkn = getHashElemNum(rp->hp);
  int probel = 0;
  int prefix_len = rp->raw_q * tau + 1;

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

  rp->sel_token_num = new int[rp->raw_doc_num];
  rp->sel_tokens = new int*[rp->raw_doc_num];
  rp->sel_pos = new int*[rp->raw_doc_num];
  rp->sel_num = new int[rp->raw_doc_num];


  for (int i = 0; i < rp->raw_doc_num; i++){
    if (useposition) {
      rp->sel_tokens[i] = rp->sorted_token_list[i];
      rp->sel_pos[i] = rp->sorted_pos_list[i];
      rp->sel_token_num[i] = rp->raw_token_num[i];
    } else {
      rp->sel_tokens[i] = rp->unique_token_list[i];
      rp->sel_pos[i] = rp->unique_pos_list[i];
      rp->sel_token_num[i] = rp->unique_token_num[i];
    }

    if (rp->sel_token_num[i] < prefix_len){
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

  for (int i = HASH_ELEM_START; i < tkn; i++)
  {
    ip->hists[i] = build_basic_hist(ip->post_len[i], ip->post_doclen_list[i], NULL);
    ip->hists_tokennum += (ip->hists[i]->bucks_num < MIN_BUCK_NUM? 0 :ip->hists[i]->bucks_num);
  }
  return ip;
}

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

int setup_query_krhash(query_t *qp, char *qry_str, int qry_len, raw_data_t *rp, int tau)
{
  int tkn = 0;
  qp -> qry_str = qry_str;
  qp -> qry_len = qry_len;
  
  memset (qry_str + qry_len, SUFFIX_PATCH_CHAR, rp->raw_q);
  tkn = qry_len + rp->raw_q + rp->raw_q - 2;
  qry_str[qry_len + rp->raw_q - 1] = '\0';
  tkn = rkRollingStringSearch(rp->hp, qry_str - rp->raw_q + 1, tkn, qp->token_id_list);

  // Now set up the prefix.
  for (int i = 0; i < tkn; i++) {
    qp -> probe_tokens[i] = qp->token_id_list[i];
    if (useposition)
      qp -> probe_pos[i] = i;
    else
      qp -> probe_pos[i] = 0;
  }
  quickSortTwoKeys(qp -> probe_tokens, qp -> probe_pos, 0, tkn);

  if (useposition == 0) {
    int current_tid = -100;
    int numunique = 0;
    for (int i = 0; i < tkn; i++) {
      if (qp -> probe_tokens[i] != current_tid) {
        qp -> probe_tokens[numunique] = qp -> probe_tokens[i];
        numunique ++;
        current_tid = qp -> probe_tokens[i];
      }
    }
    tkn = numunique;
  }
  
  if(tkn < rp->raw_q * tau + 1)
    return 0;
    
  qp->token_num = tkn;
  qry_str[qry_len] = '\0';
  return tkn;
}



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

int simple_verify(raw_data_t *rp, query_t *qp)
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
    }
  }
  return res;
}

void SymetricQgramPrefixSearch(FILE *fp, raw_data_t *rp, index_t *ip, query_t *qp, int tau)
{  
  int l;  
  char linebuf[MAX_DOC_LEN + 128];
  memset(linebuf, PREFIX_PATCH_CHAR, rp->raw_q);
  char *line = linebuf + rp->raw_q;
  int llen;

  while (fgets(line, MAX_DOC_LEN, fp)){    
    l = strlen(line);
    if (l == 0) continue;
    while ( line[l-1] == '\n' || line[l-1] == '\r' ){ line[--l] = '\0';}
    llen = l;
    queryNum ++;
    int tkn;
    if ((tkn=setup_query_krhash(qp, line, llen, rp, tau)) <= 0){
      underflowNum++;
      continue;
    }
    qp->probe_num = rp->raw_q * tau + 1;
    candNum += prefix_filtering_probe(rp, qp, ip);
    resultNum += simple_verify(rp, qp);
  }
  return;
}

int main(int argc, char* argv[])
{
  char *input = NULL;
  char c;
  int tau = 0;
  int useposition = 0;
  usage_timer_t indextimer, searchtimer;
  raw_data_t *rp;
  index_t *ip;
  query_t *qp;
  

  while ((c = getopt(argc,argv, "hvt:i:CGcg")) != -1)
    switch (c){
    case 't':
      tau = atoi(optarg);
      break;
    case 'i':
      input = optarg;
      break;
    case 'p':
      useposition = 1;
      break;
    case 'h':
      print_usage();
      break;
    case 'v':
      print_version();
      break;
    case '?':
      if ( optopt == 't' || optopt == 'i' || 'l' )
        cerr << "Error: Option -" << optopt << "requires an argument." << endl;
      else if ( isprint(optopt))
        cerr << "Error: Unknown Option -" << optopt << endl;
      else
        cerr << "Error: Unknown Option character" <<endl;
      return 1;
    default:
      print_usage();
    }
  
  if ( input == NULL ){
    cerr << "Need input file name" <<endl;
    print_usage();
  }

    candNum = 0;
  resultNum = 0;

  rp = build_raw_data_from_files(input);
  ResetUsage(&indextimer);
  ip = build_gram_index(rp, tau);
  StatUsage(&indextimer);

  qp = init_query(rp, tau);
  ResetUsage(&searchtimer);
  SymetricQgramPrefixSearch(stdin, rp, ip, qp, tau);
  StatUsage(&searchtimer);
  
  cerr << "# Q: " << rp->raw_q << endl;
  cerr << "# Tau: " << tau << endl;
  cerr << "# DataDucNum: " << rp->raw_doc_num << endl;
  cerr << "# DataUnderflow: " << ip->underflow_data<<endl;
  cerr << "# IndexedTokenNum: "<< ip->indexed_tokennum<<endl;
  cerr << "# SkipListNum: " << ip->hists_tokennum<<endl;
  cerr << "# QueryNum: " << queryNum << endl;
  cerr << "# UderflowQuery: " << underflowNum << endl;
  cerr << "# CandOneNum: " << candNum << endl;
  cerr << "# FinalResultsNum: " << resultNum << endl;  
  ShowUsage(&indextimer, "# Index", stderr);
  ShowUsage(&searchtimer, "# Search", stderr);
  
  return 0;
}


