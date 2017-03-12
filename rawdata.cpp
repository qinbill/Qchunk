// In this file, we read and manage the acture raw data.
// Raw data keep the basic datas for processing. 
// Firstly the data for chars.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashmap.h"
#include "qgrams.h"
#include "rawdata.h"
#include "sorting.h"


raw_data_t * build_raw_data_from_strings(int str_num, char **str_list, int *str_len, int q)
{
  raw_data_t *rp;  
  rp = new raw_data_t;
  int maxl = -INF, minl = INF;
  
  // initialize the data
  rp -> raw_doc_num = str_num;
  rp -> raw_q = q;  
  rp -> raw_doc_str = new char*[str_num];
  rp -> raw_doc_len = new int[str_num];
  rp -> raw_doc_id  = new int[str_num];
  rp -> raw_token_list = new int*[str_num];
  rp -> raw_token_num = new int[str_num];
  rp -> hp = initHash(q);
  
  // Start to put string data into data structure
  for (int i = 0; i < str_num; i ++ )
  {
    rp -> raw_doc_len[i] = str_len[i];
    rp -> raw_doc_str[i] = new char[str_len[i]+1];
    strncpy(rp -> raw_doc_str[i],  str_list[i], str_len[i]+1);
    rp -> raw_doc_str[i][str_len[i]] = '\0';

    if (str_len[i] > maxl)      maxl = str_len[i];
    if (str_len[i] < minl)      minl = str_len[i];

    rp -> raw_doc_id [i] = i;    
  }

  // Put the maxlenght and min length
  rp -> raw_doc_max_len = maxl;
  rp -> raw_doc_min_len = minl;
  
  // Now generate the temp data for qgram generation.
    // We initialize the memory for qglist;
  char **qglist = new char*[2 * maxl];
  int *qgidlist = new int[2 * maxl];
  char *qglistmem = new char[2 * maxl * (q + 1)];
  for (int i = 0; i < 2 * maxl; i ++ )
    qglist[i] = qglistmem + i * (q + 1);

  for (int i = 0; i < str_num; i++)
  {
    int tkn = 0;    
    tkn = doc2QgramPatch (qglist, str_list[i], str_len[i], q);
    
    // Now we need to transfer qglist into token idlist. 
    for (int j = 0; j < tkn; j++)
      qgidlist[j] = searchInsertKey(rp->hp, qglist[j]);
    
    // new a token list to keep them.
    rp -> raw_token_list[i] = new int[tkn];
    rp -> raw_token_num[i] = tkn;
    
    for (int j = 0; j < tkn; j++)
      rp -> raw_token_list[i][j] = qgidlist[j];
  }
  
  sortTokenbyFreq(rp->hp);
  
  rp -> sorted_token_list = new int*[str_num];
  rp -> sorted_pos_list =  new int*[str_num];
  rp -> unique_token_list = new int*[str_num];
  rp -> unique_pos_list = new int*[str_num];
  rp -> unique_token_num = new int[str_num];
  
  for (int i = 0; i < str_num; i++)
  {
    int tkn = rp -> raw_token_num[i];
    rp -> sorted_token_list[i] = new int[tkn];
    rp -> sorted_pos_list[i] = new int[tkn]; //rp -> sorted_token_list[i] + tkn;    
    for (int j = 0; j < tkn; j++)
    {
      rp -> raw_token_list[i][j] = rp->hp->hashElem[rp -> raw_token_list[i][j]].id;
      rp -> sorted_pos_list[i][j] = j;
    }
    memcpy (rp->sorted_token_list[i], rp->raw_token_list[i], sizeof(int)*tkn);
    quickSortTwoKeys(rp->sorted_token_list[i], rp->sorted_pos_list[i], 0, tkn-1);

    // Make Unique tid list.
    int numunique = 0;
    int current_tid = -1;
    for (int j = 0; j < tkn; j++) {
      if (rp->sorted_token_list[i][j] != current_tid) {
        numunique ++;
        current_tid = rp->sorted_token_list[i][j];
      }
    }
    rp -> unique_token_num[i] = numunique;
    rp -> unique_token_list[i] = new int[numunique];
    rp -> unique_pos_list[i] = new int[numunique];
    numunique = 0;
    current_tid = -1;
    for (int j = 0; j < tkn; j++) {
      if (rp->sorted_token_list[i][j] != current_tid) {
        rp -> unique_token_list[i][numunique] = rp->sorted_token_list[i][j];
        rp -> unique_pos_list[i][numunique] = 0;
        current_tid = rp->sorted_token_list[i][j];
        numunique ++;
      }
    }
  }
  return rp;
}


int save_raw_data_to_files(raw_data_t *rp, const char *filename)
{
  char fileex[255];
  FILE *fp;

  // This file only store the token list
  // This token list dosen't sorted based on any inforamtion.
  {
    sprintf(fileex, "%s.bin", filename);
    fp = fopen(fileex, "wb");
    
    fwrite(&rp->raw_doc_num, sizeof(int), 1, fp);  // Document Number
    fwrite(&rp->raw_q, sizeof(int), 1, fp);  // Q length
    
    for (int i = 0; i < rp->raw_doc_num; i++){
      int tkn = rp->raw_token_num[i];
      fwrite(&tkn, sizeof(int), 1, fp); // Token number
      fwrite(rp->raw_token_list[i], sizeof(int), tkn, fp); // Token id list
    }
    fclose(fp);
  }

  {
    sprintf(fileex, "%s.sbin", filename);
    fp = fopen(fileex, "wb");
    
    // fwrite(&rp->raw_doc_num, sizeof(int), 1, fp);  // Document Number
    // fwrite(&rp->raw_q, sizeof(int), 1, fp);  // Q length
    
    for (int i = 0; i < rp->raw_doc_num; i++){
      int tkn = rp->raw_token_num[i];      
      //fwrite(&tkn, sizeof(int), 1, fp); // Token number
      fwrite(rp->sorted_token_list[i], sizeof(int), tkn, fp); // Token id list
      fwrite(rp->sorted_pos_list[i], sizeof(int), tkn, fp); // Token id list
    }
    fclose(fp);
  }

  // Output the unique token ids. 
  {
    sprintf(fileex, "%s.ubin", filename);
    fp = fopen(fileex, "wb");
    
    for (int i = 0; i < rp->raw_doc_num; i++){
      int tkn = rp->unique_token_num[i];      
      fwrite(&tkn, sizeof(int), 1, fp); // Token number
      fwrite(rp->unique_token_list[i], sizeof(int), tkn, fp); // Token id list
      fwrite(rp->unique_pos_list[i], sizeof(int), tkn, fp); // Token id list
    }
    fclose(fp);
  }

  // Output string data information
  {
    sprintf(fileex, "%s.dat", filename);
    // First, output the token files. 
    fp = fopen(fileex, "wb");
    int bufsize = 0;
    fwrite(&rp->raw_doc_num, sizeof(int), 1, fp);  // Number of docs.
    for(int i = 0 ; i < rp->raw_doc_num; i++ ){      
      bufsize += (rp->raw_doc_len[i] + 1);
      fwrite(&rp->raw_doc_len[i], sizeof(int), 1, fp);
    }
    
    // Write the buffer size first
    fwrite(&bufsize, sizeof(int), 1, fp);  // The string buffer
    for(int i = 0 ; i < rp->raw_doc_num; i++ )
    {      
      fwrite(rp->raw_doc_str[i], sizeof(char), rp->raw_doc_len[i] + 1, fp);  
    }    
    fclose(fp);
  }

  // Output token information
  {    
    sprintf(fileex, "%s.tok", filename);    
    savingHash(rp->hp,fileex);
  }

  // Output the document with it's document id in sorted order  
  {
    sprintf(fileex, "%s.txt", filename);
    fp = fopen(fileex, "w");
    for (int i = 0; i < rp->raw_doc_num; i ++){
      fprintf(fp, "DID= %d DOC= \"%s\"\n", i, rp->raw_doc_str[i]);
    }
    fclose(fp);
  }


  // Free the data structure
  return 0;  
}


raw_data_t* build_raw_data_from_files(const char *filename)
{
  char fileex[255];
  FILE *fp;  
  raw_data_t *rp;
  int tkn;  
  int bufsize = 0;
  rp = new raw_data_t;

  // This file only store the token list
  // This token list dosen't sorted based on any inforamtion.  
  {
    sprintf(fileex, "%s.bin", filename);
    fp = fopen(fileex, "rb");    
    
    if(fread(&rp->raw_doc_num, sizeof(int), 1, fp)!=1)  // Document Number
      fprintf(stderr, "ERROR:readerror 1\n");

    if(fread(&rp->raw_q, sizeof(int), 1, fp)!=1)  // Q length
      fprintf(stderr, "ERROR:readerror 2\n");
    
    rp -> raw_token_list = new int*[rp->raw_doc_num];
    rp -> raw_token_num = new int[rp->raw_doc_num];
    
    for (int i = 0; i < rp->raw_doc_num; i++){
      
      if(fread(&tkn, sizeof(int), 1, fp)!=1) // Token number
        fprintf(stderr, "ERROR:readerror 3\n");
      rp -> raw_token_num[i] = tkn;      
      rp -> raw_token_list[i] = new int[tkn];      
      
      if(fread(rp->raw_token_list[i], sizeof(int), tkn, fp)!=(uint)tkn) // Token id list
        fprintf(stderr, "ERROR:readerror 4\n");
    }
    fclose(fp);
  }



  // This file only store the token list
  // This token list dosen't sorted based on any inforamtion.  
  {
    sprintf(fileex, "%s.sbin", filename);
    fp = fopen(fileex, "rb");    
    
    // if(fread(&rp->raw_doc_num, sizeof(int), 1, fp)!=1)  // Document Number
    //   fprintf(stderr, "ERROR:readerror 1\n");

    // if(fread(&rp->raw_q, sizeof(int), 1, fp)!=1)  // Q length
    //   fprintf(stderr, "ERROR:readerror 2\n");

    
    rp -> sorted_token_list = new int*[rp->raw_doc_num];
    rp -> sorted_pos_list = new int*[rp->raw_doc_num];
    //rp -> raw_token_num = new int[rp->raw_doc_num];
    
    for (int i = 0; i < rp->raw_doc_num; i++){
      int tkn = rp -> raw_token_num[i];
      rp -> sorted_token_list[i] = new int[tkn];
      rp -> sorted_pos_list[i] = new int[tkn];//rp -> sorted_token_list[i] + tkn;

      if(fread(rp->sorted_token_list[i], sizeof(int), tkn, fp)!=(uint)tkn) // Token id list
        fprintf(stderr, "ERROR:readerror 13\n");
      if(fread(rp->sorted_pos_list[i], sizeof(int), tkn, fp)!=(uint)tkn) // Token id list
        fprintf(stderr, "ERROR:readerror 14\n");      
    }
    fclose(fp);
  }

  {
    sprintf(fileex, "%s.ubin", filename);
    fp = fopen(fileex, "rb");    
    

    rp -> unique_token_num = new int[rp->raw_doc_num];
    rp -> unique_token_list = new int*[rp->raw_doc_num];
    rp -> unique_pos_list = new int*[rp->raw_doc_num];
    
    for (int i = 0; i < rp->raw_doc_num; i++){
      int tkn = rp -> raw_token_num[i];
      fread(&tkn, sizeof(int), tkn, fp); // NUmber of unique tokens.
      rp -> unique_token_num[i] = tkn;

      rp -> unique_token_list[i] = new int[tkn];
      rp -> unique_pos_list[i] = new int[tkn];//rp -> sorted_token_list[i] + tkn;

      if(fread(rp->unique_token_list[i], sizeof(int), tkn, fp)!=(uint)tkn) // Token id list
        fprintf(stderr, "ERROR:readerror 13\n");
      if(fread(rp->unique_pos_list[i], sizeof(int), tkn, fp)!=(uint)tkn) // Token id list
        fprintf(stderr, "ERROR:readerror 14\n");      
    }
    fclose(fp);
  }

  // Read string data information
  {
    sprintf(fileex, "%s.dat", filename);
    // First, output the token files. 
    fp = fopen(fileex, "rb");
    if(fread(&bufsize, sizeof(int), 1, fp)!=1)  // Number of docs.
      fprintf(stderr, "ERROR:readerror 5\n");

    rp -> raw_doc_len = new int[rp->raw_doc_num];
    rp -> raw_doc_id = new int[rp->raw_doc_num];
    rp -> raw_doc_str = new char*[rp->raw_doc_num];
    
    if(fread (rp -> raw_doc_len, sizeof(int), rp->raw_doc_num, fp)!=(uint)rp->raw_doc_num)
      fprintf(stderr, "ERROR:readerror 6\n");

    // Write the buffer size first
    if(fread(&bufsize, sizeof(int), 1, fp)!=1)  // The string buffer
      fprintf(stderr, "ERROR:readerror 7\n");
    
    for(int i = 0 ; i < rp->raw_doc_num; i++ )
    {
      rp->raw_doc_id[i] = i;
      rp->raw_doc_str[i] = new char [rp->raw_doc_len[i] + 1];
      if(fread(rp->raw_doc_str[i], sizeof(char), rp->raw_doc_len[i] + 1, fp)!=(uint)(rp->raw_doc_len[i] + 1))
        fprintf(stderr, "ERROR:readerror 8\n");
    }
    fclose(fp);
  }

  // Read token information
  {    
    sprintf(fileex, "%s.tok", filename);    
    rp->hp = readHash(fileex);  
  }
  
  {
    // BUild VCN
#ifdef VCN_FILTER_LEN
    init_vcn_map(12341);
    create_vcn_vectors(rp);
#endif
  }
  
  // Free the data structure
  return rp;  
}


int raw_data_dump(FILE *fp, raw_data_t *rp)
{
  fprintf(fp, "Q = %d doc_num = %d\n", rp->raw_q, rp->raw_doc_num);
  fprintf(fp, "\nDOC STRINGS:  \n");
  if ( rp -> raw_doc_str != NULL )
  {
    for(int i = 0; i < rp->raw_doc_num; i++)
      fprintf(fp, "DID= %d DOC=\"%s\"\n", rp->raw_doc_id[i], rp->raw_doc_str[i]);
  }
  fprintf(fp, "\nDOC TOKENS: \n");
  if ( rp -> raw_token_list != NULL)    
  {
    for(int i = 0; i < rp->raw_doc_num; i++)
    {            
      fprintf(fp, "DID= %d TKN=%d ", rp->raw_doc_id[i], rp->raw_token_num[i]);
      for(int j = 0; j < rp->raw_token_num[i]; j++)        
        fprintf(fp, "[%d|\"%s\"] ",rp->hp->order[rp->raw_token_list[i][j]], getKey(rp->hp, rp->hp->order[rp->raw_token_list[i][j]]));
      fprintf(fp, "\n");
    }    
  }
  return 0;  
}


void destroy_raw_data(raw_data_t *rp)
{
  for (int i = 0; i < rp->raw_doc_num; i++)
  {
    delete [] rp->raw_doc_str[i];
    delete [] rp->raw_token_list[i];    
  }
  
  delete [] rp->raw_doc_str;
  delete [] rp->raw_doc_len;
  delete [] rp->raw_doc_id;
  delete [] rp->raw_token_list;
  delete [] rp->raw_token_num;
  
  freeHash(rp->hp);
  delete rp;
}

