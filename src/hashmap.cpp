#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "hashmap.h"


unsigned exp_mod(unsigned int n);


hash_t* initHash(int qlen)
{
  hash_t *hp = new hash_t;
  
  // Hash start from 1.
  hp->hashElemNum = HASH_ELEM_START;
  hp->hashElem = new elem_hash_t[ELEM_NUM];
  hp->head = new int[PRIME+1];
  hp->order = NULL;
  hp->qlen = qlen;
  hp->rk_top_one = exp_mod(RK_HASH_B, qlen);  
	memset(hp->head, HASH_NOT_FOUND, (PRIME+1) * sizeof(int));		
  return hp;  
}

void resetHash(hash_t *hp)
{
	memset(hp->head, HASH_NOT_FOUND, (PRIME+1) * sizeof(int));
	for (int i = 0; i < hp->hashElemNum; i++)
		delete [] hp->hashElem[i].key;
	hp->hashElemNum = HASH_ELEM_START;
}

void freeHash(hash_t *hp)
{	
	int i;
	for (i = 0; i < hp->hashElemNum; i++) 
		delete [] hp->hashElem[i].key;
  delete [] hp->head;
  delete [] hp->order;
  delete hp;
}

unsigned bitwiseHash(const char *key) {
	unsigned h = 1159241;
	while (*key) {
		h ^= (h << 5) + (h >> 2) + *key++;
	}
	return h&0x7fffffff;
}


unsigned rkHash(hash_t *hp, const char *key)
{
  long long value = 0;
  long long power = 1;
  int len = hp->qlen;
  
  for (int i = len - 1; i >= 0; i--)
    //for (int i = 0; i < len; i++)
  {
    value += (power * key[i]);
    value %= RK_HASH_M;
    power *= RK_HASH_B;
    power %= RK_HASH_M;
  }
  return (unsigned)value;
}


unsigned exp_mod(unsigned x,  unsigned n)
{
  if (n == 0)
    return (1);
  else if (n == 1)
    return (x % RK_HASH_M);
  else 
  {
    unsigned long long square = ((long long) x * x ) % RK_HASH_M;
    unsigned long long exp = exp_mod (square, n / 2);
    
    if (n % 2 == 0)
      return (exp % RK_HASH_M);
    else
      return ((exp * x) % RK_HASH_M);
  }
}

int rkRollingStringSearch(hash_t* hp, char *str, int str_len, int *eidlist)
{
  int qlen = hp->qlen;
  unsigned topone = hp->rk_top_one;
  long thash = rkHash (hp, str);
  int i;
  
  eidlist[0] = hp->head[thash];
  //  printf( "KEYYY ");    
  // for ( int i = 0; i < qlen; i ++)
  //   putchar(str[i]);
  //  printf("  %ld\n", thash);
  

  for (i = 0; i < str_len - qlen; i++)
  {
    //printf( "KEYYY ");    
    // for ( int j = 0; j < qlen; j ++)
    //   putchar(str[i+j+1]);
    
    thash *= RK_HASH_B;
    thash  -= (((long long)str[i] * topone) % RK_HASH_M);
    thash += str[i + qlen];
    thash %= RK_HASH_M;

    // thash *= RK_HASH_B;
    // thash -= ((str[i] * topone) & (RK_HASH_M -1));
    // thash += str[i + qlen];
    // thash &= (RK_HASH_M -1);


    if (thash < 0)
      thash += RK_HASH_M;
    eidlist[i+1] = hp->head[thash];
    // long phash=rkHash(hp, str+i+1);
    // printf( "Keyss  %ld %ld\n", thash, phash);    
  }
  //  printf( "\n");    
  return i + 1;
}


void insertHash(hash_t* hp, const char *key)
{
	if (hp->hashElemNum == ELEM_NUM){fprintf(stderr, "HASH ELEM OVERFLOW\n"); return ;}
	hp->hashElem[hp->hashElemNum].key = new char [strlen(key) + 1];
	strcpy(hp->hashElem[hp->hashElemNum].key, key);
	hp->hashElem[hp->hashElemNum].freq = 1;
	hp->hashElem[hp->hashElemNum].next = HASH_NOT_FOUND;
	hp->hashElem[hp->hashElemNum].id = hp->hashElemNum;
	++hp->hashElemNum;
}

int searchInsertKey(hash_t *hp, const char *key)
{
	//int id = ELFHash(key) % PRIME;
	//int id = bitwiseHash(key) % PRIME;
	int id = rkHash(hp, key);
  int pos = hp->head[id];

	if (pos == HASH_NOT_FOUND) {
		pos = hp->head[id] = hp->hashElemNum;
    insertHash(hp, key);
    return pos;
	} else {
#ifndef OPEN_HASH
    ++hp->hashElem[pos].freq;
    return pos;
#ifdef DEBUG
    if ( strcmp(key, hp->hashElem[pos].key) != 0 )
      fprintf(stderr, "Find A HASH Conflict %s %s\n", key, hp->hashElem[pos].key);    
#endif
#else
    int last;
		while (pos > HASH_NOT_FOUND) {
			if (strcmp(hp->hashElem[pos].key, key) == 0) break;
			last = pos, pos = hp->hashElem[pos].next;
		}
		if (pos > HASH_NOT_FOUND) 
      ++hp->hashElem[pos].freq;
		else { 
			pos = hp->hashElem[last].next = hp->hashElemNum;
			insertHash(hp, key);
		}
#endif
	}
  
	//if (hashElemNum == ELEM_NUM) printf("Hash Overflow!\n");
	return pos;
}

int searchKey(hash_t *hp, const char *key)
{  
  //int id = bitwiseHash(key) % PRIME;
  int id = rkHash(hp, key);

#ifndef OPEN_HASH
  return hp->head[id];
#else
  int pos = hp->head[id];
	while (pos > HASH_NOT_FOUND) {
		if (strcmp(hp->hashElem[pos].key, key) == 0) break;    
		pos = hp->hashElem[pos].next;
	}
  return pos;
#endif
}

int getFreq(hash_t *hp, int id)
{
  if ( id < HASH_ELEM_START ) return 0;
	return hp->hashElem[id].freq;
}

int setFreq(hash_t *hp, int id, int freq)
{
	return hp->hashElem[id].freq = freq;
}

char *getKey(hash_t *hp, int id)
{
	return hp->hashElem[id].key;
}

int getHashElemNum(hash_t *hp)
{
	return hp->hashElemNum;
}


typedef struct __tokcmp
{
  int pos;
  int freq;
} tokcmp_t;

int __compfrq(const void *a, const void *b)
{
  tokcmp_t *ap = (tokcmp_t*)a;
  tokcmp_t *bp = (tokcmp_t*)b;
  
  int ret = ap->freq - bp->freq;
  
  if ( ret == 0 )
  {
    ret = ap->pos - bp->pos;
  }
  
  return ret;
}

void sortTokenbyFreq(hash_t *hp)
{
  hp->order = new int[hp->hashElemNum];
  tokcmp_t *order  = new tokcmp_t [hp -> hashElemNum];
  
  for (int i = 0; i < hp -> hashElemNum; i++)
  {
    order[i].pos = i;
    order[i].freq = hp->hashElem[i].freq;
  }

  qsort (order, hp->hashElemNum, sizeof(tokcmp_t), __compfrq);
  
  for (int i = 0; i < hp -> hashElemNum; i++)
  {
    hp->order[i] = order[i].pos;
    hp->hashElem[order[i].pos].id = i;
  }
  delete [] order;
}


int savingHash(hash_t *hp, char* file)
{
  FILE *fp;
  int i;
  int tokenfreq;
  char *key;
  int keylen;
  int next;
  int tid;
  
  fp = fopen(file, "wb");
  
  // Now put the chunk information into it:
  fwrite(&hp->hashElemNum, sizeof(int), 1, fp);  // Token number
  //fwrite(&hp->rk_top_one, sizeof(unsigned), 1, fp);
  
  for(i = HASH_ELEM_START; i < hp->hashElemNum; i++){
    if(hp->order != NULL){
      tid = hp->order[i];
    }else
    {
      tid = i;
    }

    fwrite(&hp->hashElem[tid].id, sizeof(int), 1, fp); // Token id
    key = getKey(hp, tid);
    keylen = strlen(key);    
    fwrite(&keylen, sizeof(int), 1, fp);    
    fwrite(key, sizeof(char), keylen + 1, fp); // Token string
    tokenfreq = getFreq(hp, tid);
    fwrite(&tokenfreq, sizeof(int), 1, fp); // Token string
    next = hp->hashElem[tid].next;
    fwrite(&next, sizeof(int), 1, fp); // Token string
// #ifdef DEBUG
//     fprintf(stdout, "i = %d tid = %d freq = %d  key = \"%s\"\n", 
//             i, i, tokenfreq, getKey(hp, i));
// #endif
  }
  //  fwrite(hp->head, sizeof(int), PRIME, fp);    
  fwrite(&hp->qlen, sizeof(int), 1, fp);
  fclose(fp);
  return 0;
}


hash_t *readHash(char* file)
{
  FILE *fp;
  int i;
  uint t;
  int tid;
  int tokenfreq;
  char keybuf[MAX_KEY_LEN];
  char *key = keybuf;  
  int  keylen;
  hash_t *hp;// = initHash();
  int next;
  int hashelenum, qlen;
  
  fp = fopen(file, "rb");
  

  if(fread(&hashelenum, sizeof(int), 1, fp)!=1)
    fprintf(stderr, "ERROR:readerror 10\n");  // Token number

  hp = initHash(0);
  hp->hashElemNum = hashelenum;  
  
  for(i = 1; i < hp->hashElemNum; i++){
    if((t = fread(&tid, sizeof(int), 1, fp))!=1){ // Token id, Useless now
      fprintf(stderr, "ERROR:readerror 11 %u\n", t);
      perror ("The following error occurred");
    }

    if((t=fread(&keylen, sizeof(int), 1, fp))!=1)
    {
      fprintf(stderr, "ERROR:readerror 12 %u\n", t);      
      perror ("The following error occurred");
    }

    if((t=fread(key, sizeof(char), keylen + 1, fp))!=(uint)(keylen+1)) // Token string
      fprintf(stderr, "ERROR:readerror 13 %u\n",t);
    
    if(fread(&tokenfreq, sizeof(int), 1, fp)!=1) // Token string
      fprintf(stderr, "ERROR:readerror 14\n");
    
    if(fread(&next, sizeof(int), 1, fp)!=1)      // The next pos.
      fprintf(stderr, "ERROR:readerror 15\n");
    
    hp->hashElem[i].id = tid;
    hp->hashElem[i].key = new char[keylen + 1];
    strncpy(hp->hashElem[i].key, key, keylen + 1);
    hp->hashElem[i].freq = tokenfreq;
    hp->hashElem[i].next = next;

#ifdef DEBUG
    //    fprintf(stdout, "i = %d tid = %d freq = %d  key = \"%s\"\n",
    //i, tid, tokenfreq, getKey(hp, tid));
#endif
  }

  // Now put the chunk information into it:
  if(fread(&qlen, sizeof(int), 1, fp)!=1)
    fprintf(stderr, "ERROR:readerror 10\n");  // Token number
  fclose(fp);  

  hp->qlen = qlen;
  hp->rk_top_one = exp_mod(RK_HASH_B, qlen);
  //  fread(&hp->head, sizeof(int), PRIME, fp);
  for (int i = 1; i < hp->hashElemNum; i++)
  {
    //    int id = bitwiseHash(hp->hashElem[i].key) % PRIME;
    int id = rkHash(hp, hp->hashElem[i].key);
    //fprintf(stderr, "%s, %d\n ", hp->hashElem[i].key, id);
    hp->head[id] = i;
  }
  

  return hp;
}



