#include "CacheStuff.h"

Entry::Entry()
{
	valid = dirty = false;
	tag = 0; 
}

Entry::Entry(bool valid)
{
	dirty = false;
	this->valid = valid; 
	tag = 0; 
}

