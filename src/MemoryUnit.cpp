#include "MemoryUnit.h"
#include <iostream>
#include <sstream>
#include "CacheStuff.h"

using namespace std; 

CacheResponse MemoryUnit::getLastResponse() {
	return lastResponse; 
}

// a RAM read so it must be a hit 
void MemoryUnit::read(unsigned long long address) {
	lastResponse.cycles = memoryAccessCycles;
	lastResponse.dirtyEviction = lastResponse.eviction = false;
	lastResponse.hit = true;
}

// a RAM write; has the same lastResponse as a read 
void MemoryUnit::write(unsigned long long address) {
	read(address); 
}

Cache::Cache(CacheConfig config, MemoryUnit& lowerLevel) : lowerLevel(lowerLevel) {
	this->config = config;
}

/* splits an address into its index and tag */
addressInfo Cache::splitAddress(unsigned long long address) {
	addressInfo a;
	unsigned long long blockAddress = address / (1 << config.numByteOffsetBits);
	unsigned long long setIndex = blockAddress % (1 << config.numSetIndexBits);
	unsigned long long tag = address >> (config.numByteOffsetBits + config.numSetIndexBits); 
	a.setIndex = setIndex;
	a.tag = tag;
	return  a; 
}

/* checks for a matching tag in this cache, looks down a level if not found
 * sets cache access cycles to the respective amount 
 */
void Cache::read(unsigned long long address) {
	lastResponse.cycles = config.cacheAccessCycles;
	
	addressInfo info = splitAddress(address);

	ostringstream displayInfo;
	displayInfo << address << " in L" << config.level << endl; 

	if(sets[info.setIndex].find(info.tag) != sets[info.setIndex].end()) {
		cout << "Found " << displayInfo.str(); 
		lastResponse.hit = true; 
	}
	else {
		cout << "Missed " << address << "in L" << config.level << endl; 
		lowerLevel.read(address);
	}
}

