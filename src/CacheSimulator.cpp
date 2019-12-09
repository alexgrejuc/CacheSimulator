/*
	Cache Simulator written by Alex Grejuc
	Oregon State University
	Fall Term 2019
*/

#include "CacheSimulator.h"
#include "CacheStuff.h"
#include "CacheController.h"

#include <iostream>
#include <fstream>
#include <thread>
#include <list>

using namespace std;

/*
	This function creates the cache and starts the simulator.
	Accepts core ID number, configuration info, and the name of the trace file to read.
*/
void initializeCache(list<CacheConfig> config, unsigned int memoryAccessCycles, string traceFile) {
	CacheController controller = CacheController(config, memoryAccessCycles, traceFile);
	controller.runTracefile();
}

/*
	This function accepts a configuration file and a trace file on the command line.
	The code then initializes a cache simulator and reads the requested trace file(s).
*/
int main(int argc, char* argv[]) {
	if (argc < 3) {
		cerr << "You need at least two command line arguments. You should provide a configuration file and at least one trace file." << endl;
		return 1;
	}

	cout << "Reading config file: " << argv[1] << endl;
	ifstream configFile(argv[1]);

	unsigned int numCacheLevels;
	unsigned int memoryAccessCycles; 
	unsigned int tmp;

	configFile >> numCacheLevels;
	cout << "System has " << numCacheLevels << " cache(s)." << endl;

	configFile >> memoryAccessCycles;
	cout << "System has " << memoryAccessCycles << " cycles per memory access" << endl; 

	list<CacheConfig> cacheConfigs;

	// read in the configuration for each cache 
	for(unsigned int i = 0; i < numCacheLevels; ++i) {
		CacheConfig config;
		configFile >> config.numberSets;
		configFile >> config.blockSize;
		configFile >> config.associativity;
		configFile >> tmp;
		config.rp = static_cast<ReplacementPolicy>(tmp);
		configFile >> tmp;
		config.wp = static_cast<WritePolicy>(tmp);
		configFile >> config.cacheAccessCycles;
		config.level = i + 1; 

		cacheConfigs.push_back(config);
	}

	// print configurations for each cache 
	for (auto config : cacheConfigs){
		cout << "L" << config.level << " cache: " << config.numberSets << " sets with ";
		cout << config.blockSize << " bytes in each block. N = " << config.associativity << endl;

		if (config.rp == ReplacementPolicy::Random) {
			cout << "Using random replacement protocol" << endl;
		}
		else {
			cout << "Using LRU protocol" << endl;
		}
		
		if (config.wp == WritePolicy::WriteThrough) {
			cout << "Using write-through policy" << endl;
		}
		else {
			cout << "Using write-back policy" << endl;
		}
	}

	configFile.close();
	
	string tracefile(argv[2]);
	initializeCache(cacheConfigs, memoryAccessCycles, tracefile);

	return 0;
}
