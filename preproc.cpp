#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <iomanip>
#include <stdio.h>
#include <getopt.h>
#include <sys/time.h>
#include "qgrams.h"
#include "header.h"
#include "hashmap.h"
#include "rawdata.h"




using namespace std;


char g_version[]=VERSION;

int   tok_list[MAX_DOC_LEN];       // Token length per line.
int   doc_len[MAX_DOC_NUM];       // The length of all strings.
int   doc_order[MAX_DOC_NUM];      // The length of all strings.
char* doc_str[MAX_DOC_NUM];       // The pointer of all strings.
int   doc_num = 0;                   // The number of all strings.


int   skip_sort_flag = 0;       // Set this flag to skip sort


void print_version(){
  fprintf(stderr, "Version: %s\n", g_version);
  exit(0);  
}

void print_usage(){
  fprintf(stderr, "usage: -q <qgram/qchunk length>          : This defins the Q of qchunk/qgram\n");
  fprintf(stderr, "       -o <output binary index prefix>   : The prefix of index filenames.\n");
  fprintf(stderr, "       -s    : Skip sorting documents by length.\n");
  print_version();
  exit(0);
}





// This method read all string data into string buffer.
// And count how many line there is.
static int readInputData(){
  char line[MAX_DOC_LEN];
  int l;
  
  while (fgets(line, MAX_DOC_LEN, stdin)){
    l = strlen(line);
    
    // We comment out this part cause the length limit
    // is not so important in data preperation. 
    //  if (l < lenlimit) continue;
    while ( line[l-1] == '\n' || line[l-1] == '\r' )
    { 
      // Delete all the useless end characters.
      line[--l] = '\0';         
    }
    if ( l == 0 ) continue;     // In case a empty line.
    doc_len[doc_num] = l;
    doc_str[doc_num] = new char[l+1];
    strncpy(doc_str[doc_num], line, l);
    doc_str[doc_num][l] = '\0';
    doc_num++;
  }
  return doc_num;
}



static int __cmpDoc(const void *a, const void *b)
{
  int x = *(int*)a;
  int y = *(int*)b;
  int ret = doc_len[x] - doc_len[y];
  if ( ret == 0)
    ret = strcmp(doc_str[x], doc_str[y]);
  return ret;
}



// This method sort the document by length unless
// it specified that the document is sorted. 
static void sortDocByLength()
{
  int i = 0;
  char **tmpstr = new char*[doc_num];
  int *tmplen = new int[doc_num];

  
  // mark the order array.
  for(i = 0; i < doc_num; i ++ )
  {
    tmpstr[i] = doc_str[i];
    tmplen[i] = doc_len[i];
    doc_order[i] = i;
  }
  
  if (!skip_sort_flag) // Skip sorting  
    qsort(doc_order, doc_num, sizeof(int), __cmpDoc);
  
  for(i = 0; i < doc_num; i ++ )
  {
    doc_str[i] = tmpstr[doc_order[i]];
    doc_len[i] = tmplen[doc_order[i]];    
  }

  delete [] tmpstr;  
  delete [] tmplen;
}


int main(int argc, char* argv[])
{
  int q = 2;
  char *output = NULL;
  char c;
  int lenlimit = 0;

  while ((c = getopt(argc,argv, "hvq:o:l:")) != -1)
    switch (c){
    case 'q':
      q = atoi(optarg);
      break;
    case 'l':
      lenlimit = atoi(optarg);
      break;
    case 'o':
      output = optarg;
      break;
    case 's':
      skip_sort_flag = 1;
      break;
    case 'h':
      print_usage();
      break;	  
    case 'v':
      print_version();
      break;
    case '?':
      if ( optopt == 'q' || optopt == 'o' )
        cerr << "Error: Option -" << optopt << "requires an argument." << endl;
      else if ( isprint(optopt))
        cerr << "Error: Unknown Option -" << optopt << endl;
      else
        cerr << "Error: Unknown Option character" <<endl;
      return 1;
    default:
      print_usage();
    }

  // output name check.
  if ( output == NULL ){
    cerr << "Need out put file name" <<endl;
    print_usage();
  }

  timeval indexStart, indexEnd;
  gettimeofday(&indexStart, NULL);
  cerr << "READ DOCS ..." << endl;
  readInputData();
  gettimeofday(&indexEnd, NULL);  
  cerr << "DOC READ TIME: " << setiosflags(ios::fixed) << setprecision(3) << double(indexEnd.tv_sec - indexStart.tv_sec) + double(indexEnd.tv_usec - indexStart.tv_usec) / 1e6 << endl;
  // Output to disk
  gettimeofday(&indexStart, NULL);
  cerr << "TOKENIZE ..." << endl;
  if(!skip_sort_flag)
    sortDocByLength();
  raw_data_t *rp = build_raw_data_from_strings(doc_num, doc_str, doc_len, q);
  sortTokenbyFreq(rp->hp);

#ifdef DEBUG 
  //raw_data_dump(stderr, rp);
#endif
  
  gettimeofday(&indexEnd, NULL);
  cerr << "TOKENIZE TIME: " << setiosflags(ios::fixed) << setprecision(3) << double(indexEnd.tv_sec - indexStart.tv_sec) + double(indexEnd.tv_usec - indexStart.tv_usec) / 1e6 << endl;
  cerr << "TOKEN NUM: " << getHashElemNum(rp->hp) << endl;
  
  save_raw_data_to_files(rp, output);
  return 0;
}

