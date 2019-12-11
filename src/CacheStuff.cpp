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

/*Entry::Entry(bool valid)
{
	dirty = false;
	this->valid = valid; 
	tag = 0; 
}*/

