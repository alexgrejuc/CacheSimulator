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

class CacheController {
	private:
		unsigned int globalCycles;
		unsigned int globalHits;
		unsigned int globalMisses;
		unsigned int globalEvictions;
		std::string inputFile, outputFile;

		// function to allow read or write access to the cache
		void cacheAccess(CacheResponse*, bool, unsigned long long);

		// compute the number of clock cycles used to complete a memory access
		void updateCycles(CacheResponse*, bool);

	public:
		CacheController(std::list<CacheConfig>, std::string);
		void runTracefile();
};

#endif //CACHECONTROLLER
