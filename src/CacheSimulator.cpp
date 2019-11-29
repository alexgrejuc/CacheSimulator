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

using namespace std;

/*
	This function creates the cache and starts the simulator.
	Accepts core ID number, configuration info, and the name of the trace file to read.
*/
void initializeCache(int id, CacheInfo config, string traceFile) {
	CacheController singleCore = CacheController(config, traceFile);
	singleCore.runTracefile();
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

	CacheInfo config;
	unsigned int numCacheLevels;
	unsigned int tmp;

	configFile >> numCacheLevels;
	cout << "System has " << numCacheLevels << " cache(s)." << endl;

	configFile >> config.memoryAccessCycles;
	configFile >> config.numberSets;
	configFile >> config.blockSize;
	configFile >> config.associativity;
	configFile >> tmp;
	config.rp = static_cast<ReplacementPolicy>(tmp);
	configFile >> tmp;
	config.wp = static_cast<WritePolicy>(tmp);
	configFile >> config.cacheAccessCycles;

	configFile.close();
	
	cout << config.numberSets << " sets with " << config.blockSize << " bytes in each block. N = " << config.associativity << endl;

	if (config.rp == ReplacementPolicy::Random) cout << "Using random replacement protocol" << endl;
	else cout << "Using LRU protocol" << endl;
	
	if (config.wp == WritePolicy::WriteThrough) cout << "Using write-through policy" << endl;
	else cout << "Using write-back policy" << endl;

	// For multithreaded operation you can do something like the following...
	// Note that this just shows you how to launch a thread and doesn't address
	// the complexities of how the threads communicate.
	// string tracefile(argv[2]);
	// thread t = thread(initializeCache, 0, config, tracefile);
	// t.detach();

	// For singlethreaded operation, you can use this approach:
	string tracefile(argv[2]);
	initializeCache(0, config, tracefile);

	return 0;
}
