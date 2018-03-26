#include <math.h>
#include "verify.h"
#include "header.h"


static int mat[2][MAX_DOC_LEN + 1];


int edit_distance(int xlen, char *xstr, int ylen, char *ystr, int tau)
{  
//int edit_distance(int idx, int idy, int tau) {
  //!!FIXME - LEN_LIMIT
  //if (lx < LEN_LIMIT || ly < LEN_LIMIT) return tau + 1;
  int i, j, ans; 
  //int* mat[2]; 
  int row, valid = 0;
  char *x = xstr;
  char *y = ystr;
  int lx = xlen;
  int ly = ylen;
  
  for (i = 0; i <= tau; i++) 
    mat[0][ly - i] = i;
  
  for (i = 1, row = 1; i <= lx; i++, row = !row) {
    valid = 0;
    if (i <= tau) mat[row][ly] = i;
    for (j = (i - tau >= 1 ? i - tau : 1); j <= (i + tau <= ly ? i + tau : ly); j++) {
      if (x[lx - i] == y[ly - j])
	mat[row][ly - j] = mat[!row][ly - j + 1];
      else
	mat[row][ly - j] = MIN3(j - 1 >= i - tau ? mat[row][ly - j + 1] : tau, mat[!row][ly - j + 1], j + 1 <= i + tau ? mat[!row][ly - j] : tau) + 1;
      if (mat[row][ly - j] <= tau) valid = 1;
    }
    if (!valid) break;
  }
  ans = valid ? mat[!row][0] : tau + 1;
  return ans;
}


#ifdef REAL_PROF
//#include "header.h"
static int matreal[2][MAX_DOC_LEN + 1];
//static int matreal[2][MAX_DOC_LEN + 1];

int edit_distance_real(int xlen, char *xstr, int ylen, char *ystr, int tau)
{  
//int edit_distance(int idx, int idy, int tau) {
  //!!FIXME - LEN_LIMIT
  //if (lx < LEN_LIMIT || ly < LEN_LIMIT) return tau + 1;
  int i, j, ans; 
  int lx = xlen;
  int ly = ylen;
  //int* matreal[2]; 
  int row, valid = 0;
  char *x = xstr;
  char *y = ystr;
  
  for (i = 0; i <= tau; i++) 
    matreal[0][ly - i] = i;
  
  for (i = 1, row = 1; i <= lx; i++, row = !row) {
    valid = 0;
    if (i <= tau) matreal[row][ly] = i;
    for (j = (i - tau >= 1 ? i - tau : 1); j <= (i + tau <= ly ? i + tau : ly); j++) {
      if (x[lx - i] == y[ly - j])
	matreal[row][ly - j] = matreal[!row][ly - j + 1];
      else
	matreal[row][ly - j] = MIN3(j - 1 >= i - tau ? matreal[row][ly - j + 1] : tau, matreal[!row][ly - j + 1], j + 1 <= i + tau ? matreal[!row][ly - j] : tau) + 1;
      if (matreal[row][ly - j] <= tau) valid = 1;
    }
    if (!valid) break;
  }
  ans = valid ? matreal[!row][0] : tau + 1;
  return ans+100;
}

#endif
