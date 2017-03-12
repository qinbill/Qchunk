// In this file, we read and manage the acture raw data.
// Raw data keep the basic datas for processing. 
// Firstly the data for chars.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashmap.h"
#include "qgrams.h"
#include "rawdata.h"

#include <string.h>
#include <stdio.h>

const char sstr[50][50] = {"abcdef","asdfasbad", "asdfaavadaf", 
                           "zxcvzzxcppuz","wertwnrkewbwjh",
                           "lkjanoiaosidjalsd"};

  
int Q = 3;
int main()
{

  int doc_len[10];  
  char *docs[10];
 
  for (int i = 0; i < 6; i ++ )
  {
    doc_len[i] = strlen(sstr[i]);
    docs[i] = new char [doc_len[i]+1];
    strcpy(docs[i], sstr[i]);    
  }
  
  raw_data_t *rp = build_raw_data_from_strings(6, docs, doc_len, Q);
  raw_data_dump(stdout, rp);
  save_raw_data_to_files(rp, "test_raw_data");
  destroy_raw_data(rp);
  
  rp = build_raw_data_from_files("test_raw_data");
  raw_data_dump(stdout, rp);
}





