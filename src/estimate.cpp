#include "header.h"



match_t matchs[MAX_DOC_LEN];
int matchsn = 0;
int over[MAX_DOC_LEN];
int esti[MAX_DOC_LEN];

int cmpInt(const void *a, const void *b)
{
	return *(int*)a - *(int*)b;
}

int cmpMatch(const void *a, const void *b){
  match_t *pa = (match_t*) a;
  match_t *pb = (match_t*) b;
  int ret;
  ret = (pa->cloc > pb->cloc) - (pa->cloc < pb->cloc);
  if ( ret == 0 ) {
    ret = (pa->qloc > pb->qloc) - (pa->qloc < pb->qloc);
  }
  return ret;
}

int dynanmic_matches(int q, int c, int tau){
  int i, j;
  int lb = clen[c] - tau;
  
  // Sort matchs
  qsort (matchs, sizeof(match_t), matchsn, cmpMatch);
  over[0] = 1;
  for (i = 0; i < matchsn; i++){
    j = i - 1;
    while (j >= 0 && (matchs[j].cloc == matchs[i].cloc ||
                      (matchs[j].cloc != matchs[i].cloc && 
                       matchs[j].qloc + Q > matchs[i].qloc)))
    {
      j --;
    }
    if ( j < 0 ){
      over[i] = 1;
    }else{
      over[i] = over[j] + 1;      
      if (over[i] + matchsn - i -1  < lb)
        return 0;          
    }
  }
  return over[matchsn-1];
}


//#define INF 0x7fffffff       

inline int is_compatiable(int i, int j){
  if (matchs[i].cloc < matchs[j].cloc && 
      matchs[i].qloc <= matchs[j].qloc - Q)
    return 1;
  return 0;
}

inline int ed_estimate(int i, int j){
  int ckgap, qggap, cked, qged, icloc, iqloc;
  if ( i < 0 ){
    icloc = 0;
    iqloc = 0;
  }else{
    icloc = matchs[i].cloc;
    iqloc = matchs[i].qloc;
  }
  ckgap = matchs[j].cloc - icloc;
  qggap = matchs[j].qloc - iqloc;

  // The ed for the chunk gap
  if ( i < 0){
    cked = ckgap / Q;
  }else{
    cked = (ckgap - 1) / Q;
  }

  // The ed for the length difference.
  qged = qggap > ckgap ? qggap - ckgap: ckgap - qggap;
  return qged > cked ? qged: cked;
}


inline int dynanmic_err_estimate(int q, int c, int tau){
  int i, j;
  int lb = clen[c] - tau;
  int mine, maxo;
  int test;
  
  // If matchs is less than lb. Just jump out.
  if ( matchsn < lb ) return -1;

  // If matchs is two much. estimate is costy
  // if ( matchsn >= (lb << 1)) return 0;

#ifdef DEBUG
  for (i = 0; i <= matchsn; i++){
    printf("<%d, %d, %d> ",i, matchs[i].cloc, matchs[i].qloc); 
  }
  printf("\n");
#endif

  // Sort matchs
  qsort (matchs, matchsn, sizeof(match_t), cmpMatch); 

#ifdef DEBUG
  printf("%s   %s\n", str[c], str[q]);
  for (i = 0; i <= matchsn; i++){
    printf("<%d, %d, %d> ",i, matchs[i].cloc, matchs[i].qloc); 
  }
  printf("\n");
#endif

  over[0] = 1;
  esti[0] = ed_estimate(-1, 0);
  for (i = 1; i < matchsn; i++){
    j = i - 1;
    mine = INF;
    maxo = 0;
    while (j >= 0 && j + (matchsn - i) + 1 >= lb){
      if (matchs[j].cloc != matchs[i].cloc &&
          matchs[j].qloc + Q <= matchs[i].qloc){
        if (over[j] > maxo)
          maxo = over[j];
        test = esti[j] + ed_estimate(j, i);
        if (test < mine){
          mine = test;
        }
      }
      j --;
    }
    if ( j < 0 && matchsn - i >= lb){
      if ( 1 > maxo )
        maxo  = 1;
      test = ed_estimate(-1, i);
      if (test < mine){
        mine = test;
      }
    }
    over[i] = maxo + 1;
    esti[i] = mine;
    //if ((esti[i] > tau || over[i] + matchsn - i - 1 < lb)&&(matchs[i+1].cloc != matchs[i].cloc))
    // if ((esti[i] > tau || over[i] + matchsn - i - 1 < lb))
    // {
    //   if (matchs[i+1].cloc != matchs[i].cloc)
    //     return -2;
    //   // for(int j = 0; j <=i; j++) printf("over[%d]=%d ",j, over[j]); printf("\n");
    //   // for(int j = 0; j <=i; j++) printf("esti[%d]=%d ",j, esti[j]); printf("\n");
    //   //      return -2;
    // }
    
  }
  j = matchsn - 1;
  mine = INF;
  while (j >= 0 && j + 1 >= lb){
    test = esti[j] + ed_estimate(j, matchsn);
    if (test < mine){
      mine = test;
    }
    j --;
  }
  if (mine > tau) return -3;
  return mine;
}

// Find the first in the list which lis no less than key.
static inline int bsearch ( int *list, int l, int r, int key ){
  int mid, left = l, right = r;
  int ret = right;
  
  while ( left < right ){
    mid = ( left + right ) >> 1;    
    if ( key > list[mid] ){
      left = mid + 1;
    }else if ( key < list[mid] ){
      ret = mid;
      right = mid;
    }else{
      ret = mid;
      break;
    }
  }
  while ( ret > l && list[ret-1] >= key) ret --;
  return ret;
}

// q is the query use the qgram index.
// c is the chunk query. use the qchunk index.
int overlap(int q, int c, int tau)
{
  int i, j, mis;
  int pql, pqr, pcl, pcr;
  int lq = qlen[q], lc = clen[c];
  
  pcl = 0;
  pcr = pcl + 1;
  
  pql = 0;
  pqr = pql + 1;

  matchsn = 0;
  mis = 0;

  // if ( c == 767355 && q == 858675 )
  // {
  //   fprintf(stderr, "DEBUG: CK %d ", c);
  //   for (i = 0; i< clen[c]; i++)
  //   {
  //     fprintf(stderr, "[%d|%d] ", ctoken[c][i], cloc[c][i]);      
  //   }
  //   fprintf(stderr, "\n");

  //   fprintf(stderr, "DEBUG: QK %d ", q);
  //   for (i = 0; i< qlen[q]; i++)
  //   {
  //     fprintf(stderr, "[%d|%d] ", qtoken[q][i], qloc[q][i]);      
  //   }
  //   fprintf(stderr, "\n");
  // }
  
  while (pcl < lc && pql < lq) {
    // expand pcr to find the chunk block
    while (pcr < lc && ctoken[c][pcl] == ctoken[c][pcr]) pcr ++;
    
    // Binary search 
    pql = bsearch(qtoken[q], pql, lq, ctoken[c][pcl]); 
    if ( pql >= lq )
      break;
    
    pqr = pql + 1;
    // expand pcr to find the chunk block
    while (pqr < lq && qtoken[q][pql] == qtoken[q][pqr]) pqr ++;
    
    if (qtoken [q][pql] == ctoken[c][pcl]){
      // Join them with their position info      
      for (i = pcl; i < pcr; i++) {
        while (pql < pqr && qloc[q][pql] < cloc[c][i] - tau) pql++;
        if (pql >= pqr ) {
          mis += (pcr - i);
          if ( mis > tau ) 
            return -1;
          break; // Not possible
        }
        
        if (qloc[q][pql] > cloc[c][i] + tau){
          mis ++;
          if (mis > tau) return -1;
          continue;
        }

        for (j = pql; j < pqr; j++) {
          if (qloc[q][j] > cloc[c][i] + tau) break;
          matchs[matchsn].cid = ctoken[c][pcl];
          matchs[matchsn].cloc = cloc[c][i];
          matchs[matchsn].qloc = qloc[q][j];
          matchsn ++;
        }
      }
      pql = pqr;
    } else {
      mis += (pcr - pcl);
      if ( mis > tau ) 
        return -1;    
    }
    pcl = pcr;
  }
  matchs[matchsn].cloc = slen[c];
  matchs[matchsn].qloc = slen[q];
  
  //    return 1;  
  int ret = dynanmic_err_estimate(q, c, tau);
  //  if (ret < 0)
  // if ( c == 767355 && q == 858675 )
  // {
  //   char tmps[100];    
  //   fprintf(stderr, "\n");
  //   fprintf(stderr, "\n");
  //   printf("CK:Ret %d %d [%s] QG:%d [%s]\n", ret, c, str[c], q, str[q]);
  //   for (i = 0; i <= matchsn; i++){
  //     printf("<%d, %d, %d> ",i, matchs[i].cloc, matchs[i].qloc); 
  //   }
  //   printf("\n");  
  //   fprintf(stderr, "DEBUG: CK %d ", c);
  //   for (i = 0; i< clen[c]; i++)
  //   {
  //     strncpy(tmps, str[c]+cloc[c][i], Q);
  //     tmps[Q] = '\0';
  //     fprintf(stderr, "[%d|%d|\"%s\"] ", ctoken[c][i], cloc[c][i], tmps);
  //   }
  //   fprintf(stderr, "\n");

  //   fprintf(stderr, "DEBUG: QK %d ", q);
  //   for (i = 0; i< qlen[q]; i++)
  //   {
  //     strncpy(tmps, str[q]+qloc[q][i], Q);
  //     tmps[Q] = '\0';      
  //     fprintf(stderr, "[%d|%d|\"%s\"] ", qtoken[q][i], qloc[q][i], tmps);      
  //   }
  //   fprintf(stderr, "\n");
  //   fprintf(stderr, "\n");
  //   fprintf(stderr, "\n");
  // }
  return ret;
//  return matchsn;
}

// min of three numbers.
int min(int a, int b, int c)
{
  if (a <= b && a <= c) return a;
  if (b <= a && b <= c) return b;
  return c;
}
