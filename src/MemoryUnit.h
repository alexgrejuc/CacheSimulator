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

class CacheSetNode {
	CacheSetNode* prev, *next;
	Entry entry;

public:
	CacheSetNode(CacheSetNode* prev, Entry e, CacheSetNode* next); 
};

class CacheSet2 {
	CacheSetNode* last;
	std::unordered_map<uint64_t, CacheSetNode*> map;

	public:
	void pushBack(uint64_t, Entry);
	void erase(CacheSetNode* node); 
};

class CacheSet {
public:
	// a map from tags to indexes in the deque
	// it is used to provide constant time lookup into a set 
	std::unordered_map<uint64_t, std::deque<Entry>::iterator> addressMap;
	std::deque<Entry> data;


	CacheSet();
	/*
	bool find(uint64_t tag);
	void writeAndUpdateUsage(uint64_t);
	void write();
	void erase(iterator*/

	//CacheSet* next;
	//CacheSet* prev; 
};

class Set {
public:
	virtual bool contains(uint64_t tag) = 0;
	virtual void update(uint64_t tag, Entry) = 0;
	virtual void erase(uint64_t tag) = 0; 
};

class directMappedSet : public Set {
	Entry entry; 
public:
	bool contains(uint64_t tag);
	void update(uint64_t tag, Entry);
	void erase(uint64_t tag); 
};

class Cache : public MemoryUnit {
	unsigned int hits, misses, evictions;
	CacheConfig config;
	MemoryUnit* lowerLevel; // ex: L2 if this is L1
	std::vector<CacheSet> sets; 
	addressInfo splitAddress(uint64_t address); 
	void updateLocalCounts();
	void updateCounts(); 
public:
	Cache(CacheConfig config, MemoryUnit* lowerLevel);
	void read(uint64_t address);
	void write(uint64_t address);
	void access(uint64_t address, bool isWrite);
	void say(); 
	std::string displayOperationResult();							// ex: "L1 miss eviction" 
	std::string displayLocalCounts();						// ex: "L1 Cache Hits: 10 Misses: 10..."  
};

#endif
