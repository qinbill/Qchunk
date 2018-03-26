#ifndef __PROBER_H__
#define __PROBER_H__

#include "rawdata.h"
#include "querier.h"
#include "indexer.h"


#define MAX2(a, b) (((a) > (b)) ? (a) : (b))
#define MAX3(a, b, c) (MAX2(MAX2((a),(b)),(c)))
#define MIN2(a, b) (((a) < (b)) ? (a) : (b))
#define MIN3(a, b, c) (MIN2(MIN2((a),(b)),(c)))
#define ABS(a)   (((a) < 0) ? -(a) : (a))


// This part probe the invertd index and generate the candidates.

// Probe the index.
int probing_qgrams_length(raw_data_t *rp, query_t *qp, index_t *ip);

// We use this method to do furter filtering and 
// Verification.
int filter_verify(raw_data_t *rp, query_t *qp);


#endif
