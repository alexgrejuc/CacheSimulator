/*
	Cache Simulator by Alex Grejuc
	Oregon State University
	Fall Term 2019
*/

#include "CacheController.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <cmath>
#include "MemoryUnit.h"

using namespace std;


unsigned int MemoryUnit::globalCycles;
//unsigned int MemoryUnit::globalHits;
//unsigned int MemoryUnit::globalMisses;
//unsigned int MemoryUnit::globalEvictions;

CacheController::CacheController(list<CacheConfig> cacheConfigs, unsigned int memoryAccessCycles, string traceFile) {
	// store the configuration info
	inputFile = traceFile;
	outputFile = inputFile + ".out";

	caches = list<MemoryUnit*>();
	MemoryUnit* previous = new MemoryUnit(memoryAccessCycles); 
	caches.push_front(previous);

	MemoryUnit::globalCycles = /*MemoryUnit::globalEvictions = MemoryUnit::globalHits = MemoryUnit::globalMisses =*/ 0; 
	
	for (auto config = cacheConfigs.rbegin(); config != cacheConfigs.rend(); ++config) {
		Cache* c = new Cache(*config, previous);
		caches.push_front(c); 
		previous = c; 
	}

	for (auto cache : caches) {
		cache->say(); 
	}
	
	// initialize the counters
	globalCycles = 0;
	globalHits = 0;
	globalMisses = 0;
	globalEvictions = 0;
	this->memoryAccessCycles = 0; 

	reads = writes = 0; 
}

CacheController::~CacheController() {
	for (MemoryUnit* cache : caches) {
		delete cache; 
	}
}

string CacheController::displayOperationResults() {
	string s("");

	for (auto cache : caches) {
		s += cache->displayOperationResult(); 
	}

	return s; 
}
/*
	Starts reading the tracefile and processing memory operations.
*/
void CacheController::runTracefile() {
	cout << "Input trace file: " << inputFile << endl;
	cout << "Output file name: " << outputFile << endl;
	
	// process each input line
	string line;
	// define regular expressions that are used to locate commands
	regex commentPattern("==.*");
	regex instructionPattern("I .*");
	regex loadPattern(" (L )(.*)(,[[:digit:]]+)$");
	regex storePattern(" (S )(.*)(,[[:digit:]]+)$");
	regex modifyPattern(" (M )(.*)(,[[:digit:]]+)$");

	// open the output file
	ofstream outfile(outputFile);
	// open the output file
	ifstream infile(inputFile);
	// parse each line of the file and look for commands
	while (getline(infile, line)) {
		// these strings will be used in the file output
		string opString, activityString;
		smatch match; // will eventually hold the hexadecimal address string
		unsigned long long address;
		// create a struct to track cache responses
		CacheResponse response;

		// ignore comments
		if (std::regex_match(line, commentPattern) || std::regex_match(line, instructionPattern)) {
			// skip over comments and CPU instructions
			continue;
		} else if (std::regex_match(line, match, loadPattern)) {
			cout << "Found a load op!" << endl;
			istringstream hexStream(match.str(2));
			hexStream >> std::hex >> address;
			outfile << match.str(1) << match.str(2) << match.str(3);
			cacheAccess(&response, false, address);
			outfile << " " << displayOperationResults();
			//outfile << " " << response.cycles << (response.hit ? " hit" : " miss") << (response.eviction ? " eviction" : "");
			reads++; 
		} else if (std::regex_match(line, match, storePattern)) {
			cout << "Found a store op!" << endl;
			istringstream hexStream(match.str(2));
			hexStream >> std::hex >> address;
			outfile << match.str(1) << match.str(2) << match.str(3);
			cacheAccess(&response, true, address);
			outfile << " " << displayOperationResults();
			//outfile << " " << response.cycles << (response.hit ? " hit" : " miss") << (response.eviction ? " eviction" : "");
			writes++; 
		} else if (std::regex_match(line, match, modifyPattern)) {
			cout << "Found a modify op!" << endl;
			istringstream hexStream(match.str(2));
			hexStream >> std::hex >> address;
			outfile << match.str(1) << match.str(2) << match.str(3);
			// first process the read operation
			cacheAccess(&response, false, address);
			outfile << " " << displayOperationResults() << endl;
			//string tmpString; // will be used during the file output
			//tmpString.append(response.hit ? " hit" : " miss");
			//tmpString.append(response.eviction ? " eviction" : "");
			//unsigned long long totalCycles = response.cycles; // track the number of cycles used for both stages of the modify operation
			// now process the write operation
			cacheAccess(&response, true, address);
			outfile << match.str(1) << match.str(2) << match.str(3);
			outfile << " " << displayOperationResults();
			//tmpString.append(response.hit ? " hit" : " miss");
			//tmpString.append(response.eviction ? " eviction" : "");
			//totalCycles += response.cycles;
			//outfile << " " << totalCycles << tmpString;
			reads++;
			writes++; 
		} else {
			throw runtime_error("Encountered unknown line format in trace file.");
		}
		outfile << endl;
	}

	// display all MemoryUnits except the RAM
	for(auto cache = caches.begin(); cache != --caches.end(); ++cache) {
		outfile << (*cache)->displayLocalCounts() << endl; 
	}

	// add the final cache statistics
	//outfile << "Hits: " << MemoryUnit::globalHits << " Misses: " << MemoryUnit::globalMisses << " Evictions: " << MemoryUnit::globalEvictions << endl;
	outfile << "Cycles: " << MemoryUnit::globalCycles << " Reads: " << reads << " Writes: " << writes << endl;
	
	infile.close();
	outfile.close();
}

/*
	This function allows us to read or write to the cache.
	The read or write is indicated by isWrite.
*/
void CacheController::cacheAccess(CacheResponse* response, bool isWrite, unsigned long long address) {

	isWrite ? caches.front()->write(address) : caches.front()->read(address);

	*response = caches.front()->getLastResponse(); 
	
	if (response->hit)
		cout << "Address " << std::hex << address << " was a hit." << endl;
	else
		cout << "Address " << std::hex << address << " was a miss." << endl;

	cout << "-----------------------------------------" << endl;
}

/*
	Compute the number of cycles used by a particular memory operation.
	This will depend on the cache write policy.
*/
void CacheController::updateCycles(CacheResponse* response, bool isWrite) {
	// your code should calculate the proper number of cycles
	response->cycles = 0;
}
