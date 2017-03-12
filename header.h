#ifndef __HEADER_H__
#define __HEADER_H__

//#define int64 __int64
#define ushort unsigned short 
#define INF 1 << 30
#define EPS 1e-8
#define MAX_STRING_SIZE 150*1024*1024
#define MAX_DOC_NUM 3*512*1024
#define MAX_DOC_LEN 10*1024
//#define PRIME 11999983
#define ELEM_NUM PRIME + 100

#define TOKEN_NUM ELEM_NUM
#define FREQ_LIMIT 1

#ifdef INDEX_QGRAM
//#define INDEX_PREFIX_LEN(x) (qlen[x] - clen[x] + D + 1)
#else
#define INDEX_PREFIX_LEN(x) (D + 1)
#endif


#define MAX2(a, b) (((a) > (b)) ? (a) : (b))
#define MAX3(a, b, c) (MAX2(MAX2((a),(b)),(c)))
#define MIN2(a, b) (((a) < (b)) ? (a) : (b))
#define MIN3(a, b, c) (MIN2(MIN2((a),(b)),(c)))
#define ABS(a)   (((a) < 0) ? -(a) : (a))

typedef unsigned int uint;


#endif
