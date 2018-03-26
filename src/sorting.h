#ifndef __SORTING_H__
#define __SORTING_H__


/* Quick sort with one key. Sort it acending.  */
void quickSortOneKey(int keyone[], int left, int right);

/* Quick sort with two keys. Sort it accending.  */
void quickSortTwoKeys(int keyone[], int keytwo[], int left, int right);

/* Quick Select the kth number from the key list with one key.  */
void quickSelectKOneKey(int keyone[], int left, int right, int k);

/* Quick select the kth number from the key lists with two keys. */
void quickSelectKTwoKeys(int keyone[], int keytwo[], int left, int right, int k);


#endif
