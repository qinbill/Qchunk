#ifndef __HASH_MAP__H__
#define __HASH_MAP__H__


//#define int64 __int64
//typedef unsigned short ushort
typedef unsigned int uint;

#define INF 1 << 30
#define EPS 1e-8
//#define PRIME 0x2000000
//#define PRIME 30571351
#define PRIME 119999983
#define ELEM_NUM PRIME + 100
#define HASH_NOT_FOUND 0
#define HASH_ELEM_START 1
#define MAX_KEY_LEN 128

#define RK_HASH_B 131071
//#define RK_HASH_B 257
#define RK_HASH_M PRIME


typedef struct elem_hash
{
  int id;
	char *key;
	int freq;
	int next;
} elem_hash_t;

typedef struct _hash_t
{
  elem_hash *hashElem;
  int *head;
  int *order;
  int hashElemNum;
  ///* Those are build for  */
  int qlen;
  unsigned rk_top_one;
}hash_t;


/* Create a new hashmap */
hash_t* initHash(int qlen);

/* Reset a hash map to a new one */
void resetHash(hash_t *hp);

/* Destroy or free a hash map */
void freeHash(hash_t *hp);

/* A bitwise hash function */
unsigned bitwiseHash(const char *key);

unsigned exp_mod(unsigned x,  unsigned n);

/* The rhHash Function */
unsigned rkHash(hash_t*hp, const char *key);

/* A rk roling  */
int rkRollingStringSearch(hash_t* hp, char *str, int str_len, int *eidlist);

/* Insert a key into a hash table */
void insertHash(hash_t* hp, const char *key);

/* Search a key in a hash table. Insert it if not found. */
int searchInsertKey(hash_t *hp, const char *key);

/* Search a key in a hash table */
int searchKey(hash_t *hp, const char *key);

/* Get the freqenency of a hash id */
int getFreq(hash_t *hp, int id);

/* Set the frequency of a hash id */
int setFreq(hash_t *hp, int id, int freq);

/* Get the key value of a hash id  */
char *getKey(hash_t *hp, int id);

/* Get the total number of this hash table */
int getHashElemNum(hash_t *hp);


/* Sort the token list by frequency */
void sortTokenbyFreq(hash_t *hp);


/* Saving a hash table into a file */
int savingHash(hash_t *hp, char* file);

/* Read a hash table from a file. */
hash_t *readHash(char* file);

#endif
