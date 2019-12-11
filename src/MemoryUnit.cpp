#include "MemoryUnit.h"
#include <iostream>
#include <sstream>
#include "CacheStuff.h"
#include <cmath>

using namespace std; 

CacheSet::CacheSet() {
	addressMap = unordered_map<uint64_t, deque<Entry>::iterator>();
	data = deque<Entry>(); 
}

CacheResponse MemoryUnit::getLastResponse() {
	return lastResponse; 
}

void MemoryUnit::updateGlobalCycles() {
	totalGlobalCycles += lastResponse.cycles;
	operationGlobalCycles += lastResponse.cycles; 
	/*lastResponse.hit ? globalHits++ : globalMisses++;
	if (lastResponse.eviction) globalEvictions++;*/
	//todo: update global dirty Eviction if needed 
}


// a RAM read so it must be a hit 
void MemoryUnit::access(uint64_t address, bool isWrite) {
	lastResponse.cycles = memoryAccessCycles;
	lastResponse.dirtyEviction = lastResponse.eviction = false;
	lastResponse.hit = true;
	updateGlobalCycles(); 
}

// a RAM read so it must be a hit 
void MemoryUnit::read(uint64_t address) {
	lastResponse.cycles = memoryAccessCycles;
	lastResponse.dirtyEviction = lastResponse.eviction = false;
	lastResponse.hit = true;
	updateGlobalCycles(); 
}

// a RAM write; has the same lastResponse as a read 
void MemoryUnit::write(uint64_t address) {
	read(address); 
}

void MemoryUnit::say() {
	cout << "RAM" << endl; 
}

string MemoryUnit::displayOperationResult(){
	return string(""); 
}

string MemoryUnit::displayLocalCounts() {
	return string(""); 
}

MemoryUnit::MemoryUnit() {
}

MemoryUnit::MemoryUnit(unsigned int memoryAccessCycles) {
	this->memoryAccessCycles = memoryAccessCycles; 
}

MemoryUnit::~MemoryUnit() {}

void Cache::updateLocalCounts() {
	lastResponse.hit ? hits++ : misses++;
	if (lastResponse.eviction) evictions++;
}

void Cache::updateCounts() {
	updateGlobalCycles();
	updateLocalCounts(); 
}

Cache::Cache(CacheConfig config, MemoryUnit* lowerLevel) {
	hits = misses = evictions = 0; 
	this->config = config;
	this->lowerLevel = lowerLevel; 

	sets = vector<CacheSet>();
	
	for(unsigned int i = 0; i < config.numberSets; ++i) {
		sets.emplace_back();  
	}
}

void Cache::say() {
	cout << "Cache" << endl; 
}

string Cache::displayOperationResult() {
	string s("L" + to_string(config.level));
	lastResponse.hit ? s += " hit" : s += " miss"; 
	if (lastResponse.eviction) s += " eviction";

	return s; 
}

std::string Cache::displayLocalCounts() {
	stringstream ss;
	ss << "L" << config.level << " Cache: Hits: " << hits << " Misses: " << misses << " Evictions: " << evictions; 
	return ss.str(); 
}

/* splits an address into its index and tag */
addressInfo Cache::splitAddress(uint64_t address) {
	addressInfo a;
	uint64_t blockAddress = address / config.blockSize;
	uint64_t setIndex = blockAddress % config.numberSets; 
	uint64_t tag = address / (config.numberSets * config.blockSize); // truncate the last n bits  
	a.setIndex = setIndex;
	a.tag = tag;
	return  a; 
}

/* checks for a matching tag in this cache, looks down a level if not found
 * sets cache access cycles to the respective amount 
 */
void Cache::read(uint64_t address) {
	lastResponse.cycles = config.cacheAccessCycles;
	
	addressInfo info = splitAddress(address);

	ostringstream displayInfo;
	displayInfo << "read " << address << " in L" << config.level << endl; 

	if(sets[info.setIndex].addressMap.find(info.tag) != sets[info.setIndex].addressMap.end()) {
		cout << "Found " << displayInfo.str(); 
		lastResponse.hit = true; 
	}
	else {
		cout << "Missed " << displayInfo.str(); 
		lowerLevel->read(address);
		lastResponse.hit = false; 
	}

	lastResponse.dirtyEviction = lastResponse.eviction = false;
	updateCounts(); 
}

void Cache::write(uint64_t address) {
	lastResponse.cycles = config.cacheAccessCycles;
	lastResponse.dirtyEviction = lastResponse.eviction = false; 

	addressInfo info = splitAddress(address);

	ostringstream displayInfo;
	displayInfo << "write " << address << " in L" << config.level << endl; 

	// look for tag in set
	if(sets[info.setIndex].addressMap.find(info.tag) != sets[info.setIndex].addressMap.end()) {
		lastResponse.hit = true;
		cout << "Found " << displayInfo.str(); 

		// if it is LRU then we need to move the element to the back of the deque to maintain usage order 
		if(config.rp == ReplacementPolicy::LRU) {
			Entry temp = *sets[info.setIndex].addressMap[info.tag];							// get  a copy of the entry 
			sets[info.setIndex].data.erase(sets[info.setIndex].addressMap[info.tag]);		// erase the entry
			sets[info.setIndex].data.push_back(temp);										// put the entry at the back to give it MRU status
			sets[info.setIndex].addressMap[info.tag] = --sets[info.setIndex].data.end();	// map value points to the updated position of the block 
		}

		// todo: implement write through and write back

		if(config.wp == WritePolicy::WriteBack) {
			sets[info.setIndex].addressMap[info.tag]->dirty = true;
		}
		else if(config.wp == WritePolicy::WriteThrough) {
			lowerLevel->write(address); 
		}
	}
	else {
		lastResponse.hit = false; 
		cout << "Missed " << displayInfo.str();

		lowerLevel->read(address);

		// is the set full? 
		if (sets[info.setIndex].data.size() == config.associativity) {
			lastResponse.eviction = true;
			
			deque<Entry>::iterator blockToErase;

			if (config.rp == ReplacementPolicy::LRU) {
				blockToErase = sets[info.setIndex].data.begin(); // since the deque keeps usage order, LRU is at the beginning 
			}
			else {
				//blockToErase = sets[info.setIndex].data.begin(); // todo: pick a random block
				//uint64_t randomSetIndex = rand() % config.associativity; 
			}

			if(blockToErase->dirty) {
				lastResponse.dirtyEviction = true;
				// todo 
			}

			// todo evict and dirty and all that
			sets[info.setIndex].data.erase(blockToErase);
		}

		sets[info.setIndex].data.emplace_back(true); // insert a valid block at the end 
		sets[info.setIndex].addressMap[info.tag] = --sets[info.setIndex].data.end();
	}

	updateCounts(); 
}


void Cache::access(uint64_t address, bool isWrite) {
	lastResponse.cycles = config.cacheAccessCycles;
	lastResponse.dirtyEviction = lastResponse.eviction = false; 

	addressInfo info = splitAddress(address);

	ostringstream displayInfo;
	if (isWrite) {
		displayInfo << "write";
	}
	else {
		displayInfo << "read"; 
	}
	
	displayInfo << " " << address << " in L" << config.level << endl; 

	// look for tag in set
	if(sets[info.setIndex].addressMap.find(info.tag) != sets[info.setIndex].addressMap.end()) {
		lastResponse.hit = true;
		cout << "Found " << displayInfo.str(); 

		// if it is LRU then we need to move the element to the back of the deque to maintain usage order 
		if(config.rp == ReplacementPolicy::LRU) {
			Entry temp = *sets[info.setIndex].addressMap[info.tag];							// get  a copy of the entry 
			sets[info.setIndex].data.erase(sets[info.setIndex].addressMap[info.tag]);		// erase the entry
			sets[info.setIndex].data.push_back(temp);										// put the entry at the back to give it MRU status
			sets[info.setIndex].addressMap[info.tag] = --sets[info.setIndex].data.end();	// map value points to the updated position of the block 
		}

		// todo: implement write through and write back
		if (isWrite) {
			if (config.wp == WritePolicy::WriteBack) {
				sets[info.setIndex].addressMap[info.tag]->dirty = true;
			}
			else if (config.wp == WritePolicy::WriteThrough) {
				lowerLevel->write(address);
			}
		}
	}
	else {
		lastResponse.hit = false; 
		cout << "Missed " << displayInfo.str();

		lowerLevel->read(address);

		// is the set full? 
		if (sets[info.setIndex].data.size() == config.associativity) {
			lastResponse.eviction = true;
			
			deque<Entry>::iterator blockToErase;

			if (config.rp == ReplacementPolicy::LRU) {
				blockToErase = sets[info.setIndex].data.begin(); // since the deque keeps usage order, LRU is at the beginning 
			}
			else {
				//blockToErase = sets[info.setIndex].data.begin(); // todo: pick a random block
				//uint64_t randomSetIndex = rand() % config.associativity; 
			}

			if(blockToErase->dirty) {
				lastResponse.dirtyEviction = true;
				// todo 
			}

			// todo evict and dirty and all that
			sets[info.setIndex].data.erase(blockToErase);
		}

		sets[info.setIndex].data.emplace_back(true); // insert a valid block at the end 
		sets[info.setIndex].addressMap[info.tag] = --sets[info.setIndex].data.end();
	}

	updateCounts(); 
}
