#include <string.h>
#include <stdlib.h>
#include "bitset.h"


void bit_map_init( bit_map_t *bm, int len )
{
  bm->min = 0xffffffff;
  bm->max = 0;
  bm->bitmap_len =  len / U_INT_LEN + 2;
  bm->bitmap = (unsigned int * ) malloc ( sizeof ( int ) * bm->bitmap_len );
  memset ( bm->bitmap, 0, sizeof(int) * bm->bitmap_len );
}

void bit_map_clear ( bit_map_t *bm )
{
  unsigned j,m;
  if ( bm->min != 0xffffffff){
	j = bm->min;
	m = bm->max - bm->min + 1;
	memset (&bm->bitmap[j], 0, sizeof(int) * m );	
	bm->min = 0xffffffff;
	bm->max = 0;
  }
}

int bit_map_set ( bit_map_t *bm, unsigned int y )
{
  unsigned j,m;
  j = y >> U_INT_BIT_LEN;
  m = y - ( j << U_INT_BIT_LEN );

  if ( bm->bitmap[j] & (1 << m) ) return 1;

  if ( j < bm->min ) bm->min = j;
  if ( j > bm->max ) bm->max = j;

  bm->bitmap[j] = bm->bitmap[j] | (1 << m) ;
  return 0;
}

int bit_map_check( bit_map_t *bm, int y )
{
  int j,m;
  j = y >> U_INT_BIT_LEN;
  m = y - ( j << U_INT_BIT_LEN );
  if ( bm->bitmap[j] & (1 << m) ) return 1;
  return 0;
}

