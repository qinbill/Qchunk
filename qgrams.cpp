#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <qgrams.h>


int doc2QgramPatch (char **qglist, char *s, int slen, int q){
  char *token_str;
  int l = 0, t = 0;
  int end, start;

  // Generate Q-Chunk First.
  for ( int i = 0; i < DOC_TOK_NUM(slen, q); i ++ ){
    token_str = qglist[t];
    l = 0;
    start = DOC_TOK_START(i, q);
    end   = DOC_TOK_END(i, q);    
    
    if ( start < 0 ){
      for (; l < - start; l ++ )
        token_str[l] = '#';
    }
    if (end < slen){
      strncpy(token_str + l, s + start + l, 
              q - l );
      l += q - l;
    }else{      
      strncpy(token_str + l, s + start + l, 
              q - l - (end - slen));
      l += q - l - (end - slen);      
    }
    while ( l < q ) token_str[l++] = '$';
    token_str[l] = '\0';
    t ++;
  }
  return t;
}

// #ifdef UTEST

// int main(int argc, char **argv)
// {
//   char * str = argv[2];//"abcdefghijklmnopqrst";
//   char *qglist[100];
//   int l = strlen(str);
//   int tk;
//   int q = atoi (argv[1]);
  

//   for (int i = 0; i < 100; i ++ ){
//     qglist[i] = new char [10];
//   }

//   tk = doc2QgramPatch(qglist, str, l, q);

//   fprintf(stdout, "doc: %s\n", str);
//   for ( int i = 0; i < tk; i ++ ){
//     fprintf(stdout, "tok %d:  [%s]\n", i, qglist[i]);
//   }

//   return 0;
// }

// #endif
