#include "header.h"

elem_hash *hashElem;
int head[PRIME];
int hashElemNum;

#define HASH_ELEM_START 1


typedef struct _hash_t
{
  elem_hash *hashElem;
  int head[PRIME];
  int hashElemNum;
}hash_t;

  




void initHash()
{
  // Hash start from 1. 
	hashElemNum = HASH_ELEM_START;
  hashElem = new elem_hash [ELEM_NUM];
	memset(head, HASH_NOT_FOUND, PRIME * sizeof(int));		
}

void resetHash()
{
	memset(head, HASH_NOT_FOUND, PRIME * sizeof(int));
	for (int i = 0; i < hashElemNum; i++)
		delete [] hashElem[i].key;
	hashElemNum = HASH_ELEM_START;
}

void freeHash()
{	
	int i;
	for (i = 0; i < hashElemNum; i++) 
		delete [] hashElem[i].key;
	hashElemNum = HASH_ELEM_START;
}

unsigned bitwiseHash(const char *key) {
	unsigned h = 1159241;
	while (*key) {
		h ^= (h << 5) + (h >> 2) + *key++;
	}
	return h&0x7fffffff;
}

void insertHash(const char *key)
{
	if (hashElemNum == ELEM_NUM){fprintf(stderr, "HASH ELEM OVERFLOW\n"); return ;}
	hashElem[hashElemNum].key = new char [strlen(key) + 1];
	strcpy(hashElem[hashElemNum].key, key);
	hashElem[hashElemNum].freq = 1;
	hashElem[hashElemNum].next = HASH_NOT_FOUND;
	++hashElemNum;
}

int searchInsertKey(const char *key)
{
	//int id = ELFHash(key) % PRIME;
	int id = bitwiseHash(key) % PRIME;
  int pos = head[id];

	if (pos == HASH_NOT_FOUND) {
		pos = head[id] = hashElemNum;
    insertHash(key);
    return pos;    
	} else {
#ifndef OPEN_HASH
    ++hashElem[pos].freq;
    return pos;
#ifdef DEBUG
    if ( strcmp(key, hashElem[pos].key) != 0 )
      fprintf(stderr, "Find A HASH Conflict %s %s\n", key, hashElem[pos].key);    
#endif
#else
    int last;
		while (pos > HASH_NOT_FOUND) {
			if (strcmp(hashElem[pos].key, key) == 0) break;
			last = pos, pos = hashElem[pos].next;
		}
		if (pos > HASH_NOT_FOUND) 
      ++hashElem[pos].freq;
		else { 
			pos = hashElem[last].next = hashElemNum;
			insertHash(key);
		}
#endif
	}
  
	//if (hashElemNum == ELEM_NUM) printf("Hash Overflow!\n");
	return pos;
}

int searchKey(const char *key)
{  
  int id = bitwiseHash(key) % PRIME;

#ifndef OPEN_HASH
  return head[id];
#else
  int pos = head[id];
	while (pos > HASH_NOT_FOUND) {
		if (strcmp(hashElem[pos].key, key) == 0) break;    
		pos = hashElem[pos].next;
	}
  return pos;
#endif
}

int getFreq(int id)
{
  if ( id < HASH_ELEM_START ) return 0;
	return hashElem[id].freq;
}

int setFreq(int id, int freq)
{
	return hashElem[id].freq = freq;
}

char *getKey(int id)
{
	return hashElem[id].key;
}

int getHashElemNum()
{
	return hashElemNum;
}


int savingToken(char* fileex)
{
  FILE *fp;
  int i;
  int tid;
  int tokenfreq;

  fp = fopen(fileex, "wb");
  
  // Now put the chunk information into it:
  fwrite(&hashElemNum, sizeof(int), 1, fp);  // Token number  
  
  for(i = 1; i < hashElemNum; i++){
    tid = i;  //tokensorted[i];
    fwrite(&i, sizeof(int), 1, fp); // Token id
    fwrite(getKey(tid), sizeof(char), Q + 1, fp); // Token string
    tokenfreq = getFreq(tid);
    fwrite(&tokenfreq, sizeof(int), 1, fp); // Token string

#ifdef DEBUG
    fprintf(stdout, "i = %d tid = %d freq = %d  key = \"%s\"\n", 
            i, tid, tokenfreq, getKey(tid));
#endif
    
  }  
  fclose(fp);

  return 0;
}
