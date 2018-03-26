// In this file, we read and manage the acture raw data.
// Raw data keep the basic datas for processing. 
// Firstly the data for chars.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hashmap.h"
#include "qgrams.h"
#include "rawdata.h"




void quickSortOneKey(int keyone[], int left, int right) 
{
  int i = left, j = right; 
  int tmp;
  int pivotone = keyone[(left + right) / 2];
 
  /* partition */
  while (i <= j)
  {
    while (keyone[i] < pivotone)
      ++i;
    while (keyone[j] > pivotone)
      --j;
    
    if (i <= j) 
    {
      tmp = keyone[i]; keyone[i] = keyone[j]; keyone[j] = tmp;
      ++i;--j;
    }
  };

  /* recursion */
  if (left < j)
    quickSortOneKey(keyone, left, j);
  
  if (i < right)
    quickSortOneKey(keyone, i, right);
}


void quickSortTwoKeys(int keyone[], int keytwo[], int left, int right) 
{
  int i = left, j = right; 
  int tmp;
  int pivotone = keyone[(left + right) / 2];
  int pivottwo = keytwo[(left + right) / 2];
 
  /* partition */
  while (i <= j)
  {
    while (keyone[i] < pivotone || (keyone[i] == pivotone && keytwo[i] < pivottwo))
      ++i;
    while (keyone[j] > pivotone || (keyone[j] == pivotone && keytwo[j] > pivottwo))
      --j;
    
    if (i <= j) 
    {
      tmp = keyone[i]; keyone[i] = keyone[j]; keyone[j] = tmp;
      tmp = keytwo[i]; keytwo[i] = keytwo[j]; keytwo[j] = tmp;
      ++i;--j;
    }
  };

  /* recursion */
  if (left < j)
    quickSortTwoKeys(keyone, keytwo, left, j);
  
  if (i < right)
    quickSortTwoKeys(keyone, keytwo, i, right);
}



void quickSelectKOneKey(int keyone[], int left, int right, int k) 
{
  int i = left, j = right; 
  int tmp;
  
  if ( i >= j ) return;

  int mid = rand() % (j - i) + i;
  int pivotone = keyone[mid];
 
  /* partition */
  while (i <= j)
  {
    while (keyone[i] < pivotone)
      ++i;
    while (keyone[j] > pivotone)
      --j;
    
    if (i <= j) 
    {
      tmp = keyone[i]; keyone[i] = keyone[j]; keyone[j] = tmp;
      ++i;--j;
    }
  };

  // recursion
  if (left <= k && j >= k )
    quickSelectKOneKey(keyone, left, j, k);
  else if ( i <= k && right >= k) 
    quickSelectKOneKey(keyone, i, right, k);  
}




void quickSelectKTwoKeys(int keyone[], int keytwo[], int left, int right, int k) 
{
  int i = left, j = right; 
  int tmp;
  
  if ( i >= j ) return;

  int mid = rand() % (j - i) + i;
  int pivotone = keyone[mid];
  int pivottwo = keytwo[mid];
 
  /* partition */
  while (i <= j)
  {

    while (keyone[i] < pivotone || (keyone[i] == pivotone && keytwo[i] < pivottwo))
      ++i;
    while (keyone[j] > pivotone || (keyone[j] == pivotone && keytwo[j] > pivottwo))
      --j;
    
    if (i <= j) 
    {
      tmp = keyone[i]; keyone[i] = keyone[j]; keyone[j] = tmp;
      tmp = keytwo[i]; keytwo[i] = keytwo[j]; keytwo[j] = tmp;
      ++i;--j;
    }


  };

  // recursion
  if (left <= k && j >= k )
    quickSelectKTwoKeys(keyone, keytwo, left, j, k);
  else if ( i <= k && right >= k) 
    quickSelectKTwoKeys(keyone, keytwo,  i, right, k);  
}
