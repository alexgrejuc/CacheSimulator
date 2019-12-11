#include "CacheStuff.h"

Entry::Entry(uint64_t tag, bool dirty, bool valid) {
	this->tag = tag;
	this->dirty = dirty;
	this->valid = valid; 
}

Entry::Entry()
{
	valid = dirty = false;
	tag = 0; 
}
