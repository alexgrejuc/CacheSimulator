#include "MemoryUnit.h"
#include <iostream>
#include <sstream>
#include "CacheStuff.h"
#include <cmath>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>

using namespace std; 

Set::~Set(){}

RandomSet::RandomSet(uint64_t capacity) {
	entries = list<Entry>();
	this->capacity = capacity; 
}

bool RandomSet::contains(uint64_t tag) {
	for (Entry entry : entries) {
		if (entry.tag == tag) return true; 
	}

	return false; 
}

void RandomSet::update(Entry e) {
	auto it = entries.begin(); 

	// look for the entry 
	for(; it != entries.end(); ++it) {
		if (it->tag == e.tag) break; // found 
	}

	if(it != entries.end()) {
		*it = e; 
	}
	else {
		entries.push_back(e); 
	}

#if DEBUG
	cout << "tags are:";
	for (auto entry : entries) {
		cout << " " << entry.tag; 
	}
	cout << endl; 
#endif 
}

// pop a random entry 
Entry RandomSet::pop() {
	int randIndex = rand() % entries.size();

	auto it = entries.begin(); 

	for(int i = 0; i < randIndex; i++) {
		it++; 
	}
	
	Entry popped = *it;
	entries.erase(it);
	return popped; 
}

bool RandomSet::isFull() {
	return entries.size() == capacity; 
}

LRUSet::LRUSet(uint64_t capacity) {
	entries = list<Entry>();
	this->capacity = capacity; 
}

bool LRUSet::contains(uint64_t tag) {
	for (Entry entry : entries) {
		if (entry.tag == tag) return  true; 
	}

	return false; 
}

void LRUSet::update(Entry e) {
	auto it = entries.begin(); 

	// look for the entry 
	for(; it != entries.end(); ++it) {
		if (it->tag == e.tag) break; // found 
	}

	if(it != entries.end()) {
		entries.erase(it); // erase it so we can push it to the back 
	}

	entries.push_back(e); 

#if DEBUG
	cout << "tags are:";
	for (auto entry : entries) {
		cout << " " << entry.tag; 
	}
	cout << endl;
#endif
}

Entry LRUSet::pop() {
	Entry popped = *entries.begin(); 
	entries.erase(entries.begin());
	return popped; 
}

bool LRUSet::isFull() {
	return entries.size() == capacity; 
}

DirectMappedSet::DirectMappedSet() {
	entry = Entry(); 
}

bool DirectMappedSet::contains(uint64_t tag) {
	return entry.tag == tag && entry.valid; 
}

void DirectMappedSet::update(Entry e) {
	entry = e; 
}

Entry DirectMappedSet::pop() {
	Entry temp = entry;
	entry.valid = 0;
	return temp; 
}

bool DirectMappedSet::isFull() {
	return entry.valid; 
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
/*void MemoryUnit::read(uint64_t address) {
	lastResponse.cycles = memoryAccessCycles;
	lastResponse.dirtyEviction = lastResponse.eviction = false;
	lastResponse.hit = true;
	updateGlobalCycles(); 
}*/

// a RAM write; has the same lastResponse as a read 
/*void MemoryUnit::write(uint64_t address) {
	read(address); 
}*/

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

	sets = vector<Set*>(); 

	/*if (config.associativity == 1) {
		sets = vector<Set*>(); 
	}
	else {
		
		//todo 
		sets = vector<Set*>(); 
	}*/

	if (config.associativity == 1) {
		for (unsigned int i = 0; i < config.numberSets; ++i) {
			sets.push_back(new DirectMappedSet());
		}
	}
	else if(config.rp == ReplacementPolicy::LRU) {
		for (unsigned int i = 0; i < config.numberSets; ++i) {
			sets.push_back(new LRUSet(config.associativity));
		}
	}
	else {
		for (unsigned int i = 0; i < config.numberSets; ++i) {
			sets.push_back(new RandomSet(config.associativity));
		}

		srand(time(NULL)); 
	}
}

Cache::~Cache() {
	for (auto set : sets) {
		delete set; 
	}
}

void Cache::say() {
	cout << "Cache" << config.level << " connected to ";
	lowerLevel->say(); 
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

void Cache::access(uint64_t address, bool isWrite) {
	lastResponse.cycles = config.cacheAccessCycles;
	lastResponse.dirtyEviction = lastResponse.eviction = false; 

	const addressInfo info = splitAddress(address);

#if DEBUG
	stringstream displayInfo;
	if (isWrite) {
		displayInfo << "write";
	}
	else {
		displayInfo << "read"; 
	}
	
	displayInfo << " " << address << " with set " << info.setIndex << " and tag " << info.tag << " (decimal) in L" << config.level << endl;
#endif

	// look for tag in set
	if(sets[info.setIndex]->contains(info.tag)) {
		lastResponse.hit = true;
#if DEBUG
		cout << "Found " << displayInfo.str();
#endif

		// if it is LRU then we need to update the last usage  
		if(config.rp == ReplacementPolicy::LRU) {
			Entry e = sets[info.setIndex]->pop();
			sets[info.setIndex]->update(e); 
		}
	}
	else {
		lastResponse.hit = false;
#if DEBUG
		cout << "Missed " << displayInfo.str();
#endif
		// in either a read or write miss we need to fetch the block into the cache 
		lowerLevel->access(address, false); 
		
		// is the set full? 
		if (sets[info.setIndex]->isFull()) {
			lastResponse.eviction = true;
			
			Entry erased = sets[info.setIndex]->pop();
#if DEBUG
			cout << "evicting " << erased.tag << endl; 
#endif
			if(erased.dirty) {
				assert(config.wp == WritePolicy::WriteBack); // todo: remove
#if DEBUG
				cout << "Dirty eviction";
#endif

				lastResponse.dirtyEviction = true;
				lowerLevel->access(erased.tag, true); // write the dirty block to the lower level  
			}
		}
	}

	Entry e(info.tag);
	//if (isWrite && config.wp == WritePolicy::WriteBack) e.dirty = true;
	
	if (isWrite) {
		if (config.wp == WritePolicy::WriteBack) {
			//sets[info.setIndex]->update(Entry(info.tag, true)); // set the dirty bit  
			e.dirty = true; 
		}
		else if (config.wp == WritePolicy::WriteThrough) {
			lowerLevel->access(address, true); 
		}
	}

	sets[info.setIndex]->update(e); 

	updateCounts(); 
}
