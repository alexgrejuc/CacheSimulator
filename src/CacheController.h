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
		unsigned int globalCycles;
		unsigned int globalHits;
		unsigned int globalMisses;
		unsigned int globalEvictions;
		unsigned int memoryAccessCycles; 
		std::string inputFile, outputFile;

		std::list<MemoryUnit*> caches; 
		// function to allow read or write access to the cache
		void cacheAccess(CacheResponse*, bool, unsigned long long);

		// compute the number of clock cycles used to complete a memory access
		void updateCycles(CacheResponse*, bool);

	public:
		CacheController(std::list<CacheConfig>, unsigned int memoryAccessCycles, std::string);
		~CacheController(); 
		void runTracefile();
};

#endif //CACHECONTROLLER
