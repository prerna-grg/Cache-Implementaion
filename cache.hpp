#ifndef _cache_h
#define _cache_h
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <limits.h>
using namespace std;
#define nway 1 //n way associative , 1 for direct map
#define bl_size 32 //in bytes
#define ca_size 16 //in KBs

/* 
1) LRU POLICY: Since lru has been declared as an int it has 32 bits , hence whenever valur of lru reaches INT_MAX the lru of the entire set is changed to zero
2) WRITEBACK: whenever a block with dirty bit 1 is being evicted , it will be counted as a writeback
3) the default has been set to direct , changes can be made for any kind of cache desired
 */

class Block{
	int blck[bl_size/4]; //array of ints
	int tag;
	bool dirty;
	int lru;
	bool valid;
	public:
		Block();
	friend class CacheSet;
	friend class Cache;
};

Block::Block(){
	for(int i=0 ; i<bl_size/4 ; i++) blck[i] = 0;
	valid = false;
	dirty = false;
	lru = 0;
}

class CacheSet{
	Block Set[nway]; //array of blocks
	friend class Cache;
};

class Cache{
	 CacheSet C[1024*ca_size/(bl_size*nway)]; //array of Sets
	 int CacheHit;
	 int CacheMiss;
	 int WriteBack;
	 public:
	 	Cache();
	 	int Cread(int);
	 	void Cwrite(int);
	 	int getMisses();
	 	int getHits();
	 	int getWB();
};

int Cache::getMisses(){return CacheMiss;}
int Cache::getHits(){return CacheHit;}
int Cache::getWB(){return WriteBack;}

Cache::Cache(){
	CacheHit = 0;
	CacheMiss = 0;
	WriteBack = 0;
}

int Cache::Cread(int addr){
	int off_b = (int)log2((double)bl_size);
	int in_b = (int)log2((double)1024*ca_size/(bl_size*nway));
	int t_b = 32-in_b-off_b;
	unsigned int offset = ((addr << (32-off_b-1))&0x7fffffff) >> (32-off_b-1) ;
	unsigned int index = ((addr << t_b-1)&0x7fffffff) >> (32-in_b-1);
	unsigned int _tag = ((addr >> 1)&0x7fffffff) >> (32-t_b-1);
	int flag=-1;
	for(int i=0 ; i<nway ; i++){
		if( C[index].Set[i].valid){
			if( C[index].Set[i].tag == _tag ){
				if( C[index].Set[i].lru == INT_MAX -1){
					for(int k=0 ; k<nway ; k++)C[index].Set[k].lru = 0;
				}
				CacheHit++;C[index].Set[i].lru++;return C[index].Set[i].blck[offset/4];
			}
		}else if(flag==-1)flag=i;
	}
	CacheMiss++;
	if( flag!=-1){
		C[index].Set[flag].tag = _tag;
		C[index].Set[flag].valid = true;
		if( C[index].Set[flag].lru == INT_MAX -1){
			for(int k=0 ; k<nway ; k++)C[index].Set[k].lru = 0;
		}
		C[index].Set[flag].lru++;
		return C[index].Set[flag].blck[offset/4];
	}
	else{
		int minlru=C[index].Set[0].lru , minindex=0;
		for(int i=1 ; i<nway ; i++){
			if( minlru > C[index].Set[i].lru ){minlru = C[index].Set[i].lru;minindex = i;}
		}
		if(C[index].Set[minindex].dirty == true )WriteBack++;
		C[index].Set[minindex].tag = _tag;
		C[index].Set[minindex].valid = true;
		if( C[index].Set[minindex].lru == INT_MAX -1){
			for(int k=0 ; k<nway ; k++)C[index].Set[k].lru = 0;
		}
		C[index].Set[minindex].lru++;
		C[index].Set[minindex].dirty = false;
		return C[index].Set[minindex].blck[offset/4];
	}
	return 0;
}

void Cache::Cwrite(int addr){
	int off_b = (int)log2((double)bl_size);
	int in_b = (int)log2((double)1024*ca_size/(bl_size*nway));
	int t_b = 32-in_b-off_b;
	unsigned int offset = ((addr << (32-off_b-1))&0x7fffffff) >> (32-off_b-1) ;
	unsigned int index = ((addr << t_b-1)&0x7fffffff) >> (32-in_b-1);
	unsigned int _tag = ((addr >> 1)&0x7fffffff) >> (32-t_b-1);
	int flag=-1;
	for(int i=0 ; i<nway ; i++){
		if( C[index].Set[i].valid){
			if( C[index].Set[i].tag == _tag ){
				if( C[index].Set[i].lru == INT_MAX -1){
					for(int k=0 ; k<nway ; k++)C[index].Set[k].lru = 0;
				}
				CacheHit++;C[index].Set[i].lru++; C[index].Set[i].dirty = true;return;
			}
		}else if(flag==-1)flag=i;
	}
	CacheMiss++;
	if( flag!=-1){
		C[index].Set[flag].tag = _tag;
		C[index].Set[flag].valid = true;
		if( C[index].Set[flag].lru == INT_MAX -1){
			for(int k=0 ; k<nway ; k++)C[index].Set[k].lru = 0;
		}
		C[index].Set[flag].lru++;
		C[index].Set[flag].dirty = true;
		return;
	}
	else{
		int minlru=C[index].Set[0].lru , minindex=0;
		for(int i=1 ; i<nway ; i++){
			if( minlru > C[index].Set[i].lru )minlru = C[index].Set[i].lru;minindex = i;
		}
		if(C[index].Set[minindex].dirty == true )WriteBack++;
		C[index].Set[minindex].tag = _tag;
		C[index].Set[minindex].valid = true;
		if( C[index].Set[minindex].lru == INT_MAX -1){
			for(int k=0 ; k<nway ; k++)C[index].Set[k].lru = 0;
		}
		C[index].Set[minindex].lru++;
		C[index].Set[minindex].dirty = true;
		return;
	}
}
#endif
