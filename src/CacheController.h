/*
	Cache Simulator (Starter Code) by Justin Goins
	Oregon State University
	Fall Term 2019
*/

#ifndef _CACHECONTROLLER_H_
#define _CACHECONTROLLER_H_

#include "CacheStuff.h"
#include <string>
#include <list>
#include "MemoryUnit.h"

class CacheController {
	private:
		CacheController & operator=(const CacheController&) = delete;
		CacheController(const CacheController&) = delete;
		CacheController() = default;
		unsigned int globalCycles;
		unsigned int globalHits;
		unsigned int globalMisses;
		unsigned int globalEvictions;
		unsigned int memoryAccessCycles;
		unsigned int reads, writes; 
		std::string inputFile, outputFile;

		std::list<MemoryUnit*> caches; 
		// function to allow read or write access to the cache
		void cacheAccess(CacheResponse*, bool, uint64_t);

		// compute the number of clock cycles used to complete a memory access
		void updateCycles(CacheResponse*, bool);
		void displayOperationResults(std::ofstream&); 

	public:
		CacheController(std::list<CacheConfig>, unsigned int memoryAccessCycles, std::string);
		~CacheController(); 
		void runTracefile();
};

#endif //CACHECONTROLLER
