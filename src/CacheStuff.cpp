#include "CacheStuff.h"

Entry::Entry(uint64_t tag, bool dirty) {
	this->tag = tag;
	this->dirty = dirty; 
}

/*Entry::Entry()
{
	valid = dirty = false;
	tag = 0; 
}

Entry::Entry(bool valid)
{
	dirty = false;
	this->valid = valid; 
	tag = 0; 
}*/

