#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "qgrams.h"

int main(int argc, char **argv)
{
  char * str = argv[2];//"abcdefghijklmnopqrst";
  char *qglist[100];
  int l = strlen(str);
  int tk;
  int q = atoi (argv[1]);
  

  for (int i = 0; i < 100; i ++ ){
    qglist[i] = new char [10];
  }

  tk = doc2QgramPatch(qglist, str, l, q);

  fprintf(stdout, "doc: %s\n", str);
  for ( int i = 0; i < tk; i ++ ){
    fprintf(stdout, "tok %d:  [%s]\n", i, qglist[i]);
  }

  return 0;
}
