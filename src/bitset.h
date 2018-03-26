#ifndef __BITSET_H__
#define __BITSET_H__



/* For bit map set */
#define U_INT_LEN 32
#define U_INT_BIT_LEN 5

typedef struct _bit_map_t{
  unsigned int min;
  unsigned int max;
  unsigned int *bitmap;
  unsigned int bitmap_len;
} bit_map_t;

/* Bitmap */
void bit_map_init(bit_map_t*, int);
void bit_map_clear (bit_map_t*);
int bit_map_set (bit_map_t*, unsigned int);
int bit_map_check(bit_map_t*, int);


#endif

