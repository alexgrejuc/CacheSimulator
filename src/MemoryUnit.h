#include "CacheStuff.h"
#include <vector>
#include <unordered_map>
#include <deque>
#ifndef _MEMORY_UNIT_
#define _MEMORY_UNIT_


class MemoryUnit {
	unsigned int memoryAccessCycles;
protected:
	CacheResponse lastResponse;
	void updateGlobalCycles(); 
	
public:
	static unsigned int globalCycles;

	virtual void read(unsigned long long address); 
	virtual void write(unsigned long long address);
	CacheResponse getLastResponse();
	MemoryUnit(unsigned int memoryAccessCycles); 
	MemoryUnit();
	virtual ~MemoryUnit();
	virtual std::string displayOperationResult(); 
	virtual std::string displayLocalCounts(); 
	virtual void say(); 
};

class CacheSet {
public:
	// a map from tags to indexes in the deque
	// it is used to provide constant time lookup into a set 
	std::unordered_map<unsigned long long, std::deque<Entry>::iterator> addressMap;
	std::deque<Entry> data;


	CacheSet();
	/*
	bool find(unsigned long long tag);
	void writeAndUpdateUsage(unsigned long long);
	void write();
	void erase(iterator*/
};

class Cache : public MemoryUnit {
	unsigned int hits, misses, evictions;
	CacheConfig config;
	MemoryUnit* lowerLevel; // ex: L2 if this is L1
	std::vector<CacheSet> sets; 
	addressInfo splitAddress(unsigned long long address); 
	void updateLocalCounts();
	void updateCounts(); 
public:
	Cache(CacheConfig config, MemoryUnit* lowerLevel);
	void read(unsigned long long address);
	void write(unsigned long long address);
	void say(); 
	std::string displayOperationResult();							// ex: "L1 miss eviction" 
	virtual std::string displayLocalCounts();						// ex: "L1 Cache Hits: 10 Misses: 10..."  
};

#endif
