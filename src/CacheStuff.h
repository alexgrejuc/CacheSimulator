/*
	Cache Simulator (Starter Code) by Justin Goins
	Oregon State University
	Fall Term 2019
*/

#ifndef _CACHESTUFF_H_
#define _CACHESTUFF_H_
#include <ctime>

enum class ReplacementPolicy {
	Random,
	LRU
};

enum class WritePolicy {
	WriteThrough,
	WriteBack
};

struct addressInfo {
	unsigned long long tag;
	unsigned long long setIndex; 
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
		Entry(); 
		Entry(bool valid); 
		bool valid;
		bool dirty;
		unsigned long long tag;
};

class TimedEntry : public Entry
{
	public:
		clock_t lastUsed;
		TimedEntry(); 
};

inline TimedEntry::TimedEntry()
{
	lastUsed = 0; 
}


#endif //CACHESTUFF
