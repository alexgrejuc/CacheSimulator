/*
	Cache Simulator (Starter Code) by Justin Goins
	Oregon State University
	Fall Term 2019
*/

#ifndef _CACHESTUFF_H_
#define _CACHESTUFF_H_
#include <ctime>
#include <cstdint>

enum class ReplacementPolicy {
	Random,
	LRU
};

enum class WritePolicy {
	WriteThrough,
	WriteBack
};

struct addressInfo {
	uint64_t tag;
	uint64_t setIndex; 
};

// structure to hold information about a particular cache
struct CacheConfig {
	unsigned int numByteOffsetBits;
	unsigned int numSetIndexBits;
	unsigned int numberSets; // how many sets are in the cache
	unsigned int blockSize; // size of each block in bytes
	unsigned int associativity; // the level of associativity (N)
	ReplacementPolicy rp;
	WritePolicy wp;
	unsigned int cacheAccessCycles;
	unsigned int level; // the level of the cache (ex: L1, L2) 
	//unsigned int memoryAccessCycles;
};

// this structure is filled with information about each memory access
struct CacheResponse {
	bool hit; // did this memory operation encounter a hit?
	bool eviction; // did this memory operation involve an eviction?
	bool dirtyEviction; // was the evicted block marked as dirty?
	unsigned int cycles; // how many clock cycles did this operation take?
};

class Entry
{
	public: 
		//Entry(); 
		//Entry(bool valid); 
		Entry(uint64_t tag, bool dirty = false); 
		bool valid;
		bool dirty;
		uint64_t tag;
};

#endif //CACHESTUFF
