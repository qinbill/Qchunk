#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <stdlib.h>
#include <iomanip>
#include <getopt.h>
#include <string.h>
#include "qgrams.h"
#include "hashmap.h"
#include "rawdata.h"
#include "indexer.h"
#include "querier.h"
#include "prober.h"
#include "header.h"
#include "dpselect.h"
#include "usage.h"


char g_version[]=VERSION;

using namespace std;


int candNum = 0;
int resultNum = 0;
int queryNum = 0;
int underflowNum = 0;


void print_version(){
  fprintf(stderr, "Version: %s\n", g_version);
}

void print_usage(){
  fprintf(stderr, "usage: -t <Max Edit Distance>    :Edit distance threshold for index building.>\n");
  //  fprintf(stderr, "       -d <Query Edit Distance threshold>   : The edit distance for this query.>\n");
  fprintf(stderr, "       -i <input file name>      :input binary file prefix\n");
  fprintf(stderr, "       -G Use Algoritm indexGramSuper (Default)\n");
  fprintf(stderr, "       -C Use Algoritm indexChunkSuper \n");  
  fprintf(stderr, "       -g Use Algoritm indexGramTurbo (Default)\n");
  fprintf(stderr, "       -c Use Algoritm indexChunkTurbo \n");
  fprintf(stderr, "       -b Use Algoritm indexChunkTurbine \n");
  print_version();
  exit(0);
}

void indexGramSuperSearch(FILE *fp, raw_data_t *rp, index_t *ip, query_t *qp, int tau)
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
    // if (setup_query_krhash(qp, line, llen, rp, tau) == 0){
    //   underflowNum++;
    //   continue;
    // }
    // cal_probe_cost(qp, ip);
    // qp->probe_num = dynamic_select_probe_tokens(qp->token_costs, qp->token_num, qp->probe_pos, tau, rp->raw_q);
    // for(int i = 0; i <= tau + 1; i++) qp->probe_tokens[i] = qp->token_id_list[qp->probe_pos[i]];
    if(indexGramSuperQueryPreprocessing(line, llen, rp, ip, qp, tau)<=0)
      continue;
    candNum += probing_qgrams_length(rp, qp, ip);
    resultNum += filter_verify(rp, qp);
  }
  return;  
}



void indexChunkSuperSearch(FILE *fp, raw_data_t *rp, index_t *ip, query_t *qp, int tau)
{  
  int l;  
  char linebuf[MAX_DOC_LEN + 128];
  memset(linebuf, PREFIX_PATCH_CHAR, rp->raw_q);
  char *line = linebuf + rp->raw_q;
  int llen;
  int *problist;
  

  for (int i = 0; i < qp->max_len; i++)
    qp->probe_pos[i] = i;

  problist = qp->probe_tokens;
  qp->probe_tokens = qp->token_id_list;

  while (fgets(line, MAX_DOC_LEN, fp)){    
    l = strlen(line);
    if (l == 0) continue;
    while ( line[l-1] == '\n' || line[l-1] == '\r' ){ line[--l] = '\0';}
    llen = l;
    queryNum ++;    
    //setup_query_krhash(qp, line, llen, rp, tau);
    // if (setup_query_krhash(qp, line, llen, rp, tau) == 0){
    //   underflowNum++;
    //   continue;
    // }

    // cal_probe_cost(qp, ip);
    // qp->probe_num = qp -> token_num;
    
    if(indexChunkSuperQueryPreprocessing(line, llen, rp, ip, qp, tau)<=0)
      continue;
    
    
    candNum += probing_qgrams_length(rp, qp, ip);
    resultNum += filter_verify(rp, qp);    
  }
  qp->probe_tokens = problist;
  return;  
}










void indexGramTurboSearch(FILE *fp, raw_data_t *rp, index_t *ip, query_t *qp, int tau)
{  
  int l;  
  char linebuf[MAX_DOC_LEN + 128];
  memset(linebuf, PREFIX_PATCH_CHAR, rp->raw_q);
  char *line = linebuf + rp->raw_q;
  //  char line[MAX_DOC_LEN];
  //char line[MAX_DOC_LEN];
  int llen;

  while (fgets(line, MAX_DOC_LEN, fp)){    
    l = strlen(line);
    if (l == 0) continue;
    while ( line[l-1] == '\n' || line[l-1] == '\r' ){ line[--l] = '\0';}
    llen = l;
    queryNum ++;
    // //setup_query_krhash(qp, line, llen, rp, tau);
    // if (setup_query_krhash(qp, line, llen, rp, tau) == 0){
    //   underflowNum++;
    //   continue;
    // }

    // cal_prefix_cost(qp, ip, tau, rp->raw_q);
    if(indexGramTurboQueryPreprocessing(line, llen, rp, ip, qp, tau)<=0)
      continue;        
    candNum += probing_qgrams_length(rp, qp, ip);
    resultNum += filter_verify(rp, qp);
  }
  return;
}



void indexChunkTurboSearch(FILE *fp, raw_data_t *rp, index_t *ip, query_t *qp, int tau)
{  
  int l;
  char linebuf[MAX_DOC_LEN + 128];
  memset(linebuf, PREFIX_PATCH_CHAR, rp->raw_q);
  char *line = linebuf + rp->raw_q;
  int llen;
  int *problist;
  
  for (int i = 0; i < qp->max_len; i++)
    qp->probe_pos[i] = i;

  problist = qp->probe_tokens;
  qp->probe_tokens = qp->token_id_list;

  while (fgets(line, MAX_DOC_LEN, fp)){    
    l = strlen(line);
    if (l == 0) continue;
    while ( line[l-1] == '\n' || line[l-1] == '\r' ){ line[--l] = '\0';}
    llen = l;
    queryNum ++;
    //setup_query_krhash(qp, line, llen, rp, tau);
    // if (setup_query_krhash(qp, line, llen, rp, tau) == 0){
    //   underflowNum++;
    //   continue;
    // }
    // cal_prefix(qp, ip, tau, rp->raw_q);

    if(indexChunkTurboQueryPreprocessing(line, llen, rp, ip, qp, tau)<=0)
      continue;

    candNum += probing_qgrams_length(rp, qp, ip);
    resultNum += filter_verify(rp, qp);
  }
  qp->probe_tokens = problist;  
  return;
}


void indexChunkTurbineSearch(FILE *fp, raw_data_t *rp, index_t *ip, query_t *qp, int tau)
{  
  int l;
  char linebuf[MAX_DOC_LEN + 128];
  memset(linebuf, PREFIX_PATCH_CHAR, rp->raw_q);
  char *line = linebuf + rp->raw_q;
  int llen;
  int *problist;
  
  for (int i = 0; i < qp->max_len; i++)
    qp->probe_pos[i] = i;

  problist = qp->probe_tokens;
  qp->probe_tokens = qp->token_id_list;

  while (fgets(line, MAX_DOC_LEN, fp)){    
    l = strlen(line);
    if (l == 0) continue;
    while ( line[l-1] == '\n' || line[l-1] == '\r' ){ line[--l] = '\0';}
    llen = l;
    queryNum ++;
    //setup_query_krhash(qp, line, llen, rp, tau);
    // if (setup_query_krhash(qp, line, llen, rp, tau) == 0){
    //   underflowNum++;
    //   continue;
    // }
    // cal_prefix(qp, ip, tau, rp->raw_q);

    if(indexChunkTurboQueryPreprocessing(line, llen, rp, ip, qp, tau)<=0)
      continue;

    candNum += probing_qgrams_length(rp, qp, ip);
    resultNum += filter_verify(rp, qp);
  }
  qp->probe_tokens = problist;  
  return;
}






int main(int argc, char* argv[])
{
  char *input = NULL;
  char c;
  int tau = 0;
  int index_gram_super = 1;     // Default
  int index_chunk_super = 0;
  int index_gram_turbo = 0;
  int index_chunk_turbo = 0;
  int index_chunk_turbine = 0;
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
    case 'C':
      index_chunk_super = 1;
      index_gram_super = 0;
      index_gram_turbo = 0;
      index_chunk_turbo = 0;
      index_chunk_turbine = 0;
      break;
    case 'G':
      index_gram_super = 1;
      index_chunk_super = 0;
      index_gram_turbo = 0;
      index_chunk_turbo = 0;
      index_chunk_turbine = 0;
      break;
    case 'c':
      index_chunk_super = 0;
      index_gram_super = 0;
      index_gram_turbo = 0;
      index_chunk_turbo = 1;
      index_chunk_turbine = 0;
      break;
    case 'g':
      index_gram_super = 0;
      index_chunk_super = 0;
      index_gram_turbo = 1;
      index_chunk_turbo = 0;
      index_chunk_turbine = 0;
      break;
    case 'b':
      index_gram_super = 0;
      index_chunk_super = 0;
      index_gram_turbo = 0;
      index_chunk_turbo = 0;
      index_chunk_turbine = 1;
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

    //line = str[n];
  //queryNum = 0;
  candNum = 0;
  resultNum = 0;

  // Use different algoritm for diff option.
  if (index_gram_super)
  {
    cerr << "# Apply IndexGramSuper Algorithm" << endl;
    rp = build_raw_data_from_files(input);
    ResetUsage(&indextimer);
    ip = build_index_qgram_super_length(rp, tau);
    StatUsage(&indextimer);
    //dump_index(stdout, ip, rp);
    qp = init_query(rp, tau);
    ResetUsage(&searchtimer);    
    dynamic_select_init(rp->raw_doc_len[rp->raw_doc_num-1] + 128, tau);
    indexGramSuperSearch(stdin, rp, ip, qp, tau);
    StatUsage(&searchtimer);

  }else if(index_chunk_super)
  {
    cerr << "# Apply IndexChunkSuper Algorithm" << endl;
    rp = build_raw_data_from_files(input);
    ResetUsage(&indextimer);
    dynamic_select_init(rp->raw_doc_len[rp->raw_doc_num-1] + 128, tau);
    ip = build_index_chunk_super_length(rp, tau);
    //dump_index(stdout, ip, rp);
    StatUsage(&indextimer);
    qp = init_query(rp, tau);
    ResetUsage(&searchtimer);
    indexChunkSuperSearch(stdin, rp, ip, qp, tau);
    StatUsage(&searchtimer);

  }else if(index_gram_turbo){
    cerr << "# Apply IndexGramTurbo Algorithm" << endl;
    rp = build_raw_data_from_files(input);
    dynamic_select_init(rp->raw_doc_len[rp->raw_doc_num-1] + 128, tau);
    ResetUsage(&indextimer);
    ip = build_index_gram_turbo_length(rp, tau);
    StatUsage(&indextimer);
    //dump_index(stdout, ip, rp);

    qp = init_query(rp, tau);
    ResetUsage(&searchtimer);
    indexGramTurboSearch(stdin, rp, ip, qp, tau);
    StatUsage(&searchtimer);

  }else if(index_chunk_turbo){
    cerr << "# Apply IndexChunkTurbo Algorithm" << endl;
    rp = build_raw_data_from_files(input);
    dynamic_select_init(rp->raw_doc_len[rp->raw_doc_num-1] + 128, tau);
    ResetUsage(&indextimer);
    ip = build_index_chunk_turbo_length(rp, tau);
    StatUsage(&indextimer);
    //dump_index(stdout, ip, rp);
    qp = init_query(rp, tau);
    ResetUsage(&searchtimer);
    indexChunkTurboSearch(stdin, rp, ip, qp, tau);
    StatUsage(&searchtimer);
  }else if(index_chunk_turbine){
    cerr << "# Apply IndexChunkTurbo Algorithm" << endl;
    rp = build_raw_data_from_files(input);
    dynamic_select_init(rp->raw_doc_len[rp->raw_doc_num-1] + 128, tau);
    ResetUsage(&indextimer);
    ip = build_index_chunk_turbine_length(rp, tau);
    StatUsage(&indextimer);
    //dump_index(stdout, ip, rp);
    qp = init_query(rp, tau);
    ResetUsage(&searchtimer);
    indexChunkTurbineSearch(stdin, rp, ip, qp, tau);
    StatUsage(&searchtimer);
  }else{
    print_usage();  
    exit(-1);
  }

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


