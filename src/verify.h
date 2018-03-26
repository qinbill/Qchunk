#ifndef __VERIFY_H__
#define __VERIFY_H__

#define MAX2(a, b) (((a) > (b)) ? (a) : (b))
#define MAX3(a, b, c) (MAX2(MAX2((a),(b)),(c)))
#define MIN2(a, b) (((a) < (b)) ? (a) : (b))
#define MIN3(a, b, c) (MIN2(MIN2((a),(b)),(c)))
#define ABS(a)   (((a) < 0) ? -(a) : (a))

/* Verify the edit distance of two string and see if they satisfy a */
/* given threshold */
int edit_distance(int xlen, char *xstr, int ylen, char *ystr, int tau);

#ifdef REAL_PROF
int edit_distance_real(int xlen, char *xstr, int ylen, char *ystr, int tau);
#endif


#endif
