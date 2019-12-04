#include "src/CacheStuff.h"
#include <vector>
#include <unordered_map>
#ifndef _MEMORY_UNIT_
#define _MEMORY_UNIT_


class MemoryUnit {
	unsigned int memoryAccessCycles; 

protected:
	CacheResponse lastResponse;
	
public:
	void read(unsigned long long address); 
	void write(unsigned long long address);
	CacheResponse getLastResponse(); 
};

class Cache : public MemoryUnit {
	CacheConfig config;
	MemoryUnit& lowerLevel; // ex: L2 if this is L1
	std::vector<std::unordered_map<unsigned long long, Entry> > sets;
	addressInfo splitAddress(unsigned long long address); 

public:
	Cache(CacheConfig config, MemoryUnit& lowerLevel); 
	void read(unsigned long long address);
	void write(unsigned long long address);
};

#endif
