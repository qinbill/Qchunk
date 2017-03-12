#ifndef __DPSELECT_H__
#define __DPSELECT_H__

#include "rawdata.h"
#include "qgrams.h"
#include "querier.h"
#include "indexer.h"


// Select the tokens for probing. 
/* Fixme need to reconsider the interface */
int dynamic_select_probe_tokens(int *costs, int costlen, 
                                int *sel_pos, int tau, int q);



int dynamic_select_probe_tokens_with_poss(int *costs, int *poss, int costlen, 
                                          int *sel_pos, int tau, int q);



/* Init the select accessory data structrue */
int dynamic_select_init(int toknum, int tau);


#endif
