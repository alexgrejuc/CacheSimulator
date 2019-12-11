#include "CacheStuff.h"
#include <vector>
#include <unordered_map>
#include <deque>
#include <memory>
#ifndef _MEMORY_UNIT_
#define _MEMORY_UNIT_


class MemoryUnit {
	unsigned int memoryAccessCycles;
protected:
	CacheResponse lastResponse;
	void updateGlobalCycles(); 
	
public:
	static unsigned int totalGlobalCycles;
	static unsigned int operationGlobalCycles;

	virtual void read(uint64_t address); 
	virtual void write(uint64_t address);
	virtual void access(uint64_t address, bool isWrite);
	CacheResponse getLastResponse();
	MemoryUnit(unsigned int memoryAccessCycles); 
	MemoryUnit();
	virtual ~MemoryUnit();
	virtual std::string displayOperationResult(); 
	virtual std::string displayLocalCounts(); 
	virtual void say(); 
};

class Set {
public:
	virtual bool contains(uint64_t tag) =0;
	virtual void update(Entry) =0;
	virtual Entry pop() =0; 
	virtual bool isFull() =0;
};

class DirectMappedSet : public Set {
	Entry entry; 
public:
	bool contains(uint64_t tag);
	void update(Entry);
	Entry pop(); 
	bool isFull();
	DirectMappedSet(); 
};

class Cache : public MemoryUnit {
	unsigned int hits, misses, evictions;
	CacheConfig config;
	MemoryUnit* lowerLevel; // ex: L2 if this is L1
	std::vector<Set*> sets; 
	addressInfo splitAddress(uint64_t address); 
	void updateLocalCounts();
	void updateCounts(); 
public:
	Cache(CacheConfig config, MemoryUnit* lowerLevel);
	void access(uint64_t address, bool isWrite);
	void say(); 
	std::string displayOperationResult();							// ex: "L1 miss eviction" 
	std::string displayLocalCounts();						// ex: "L1 Cache Hits: 10 Misses: 10..."  
};

#endif
