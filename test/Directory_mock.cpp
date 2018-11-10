#include "Directory.h"

#include "CppUTestExt/MockSupport.h"


Directory::Directory()
{
}

Directory::~Directory()
{
}

int Directory::store_key(uint64_t key, uint32_t block, size_t bytes, uint64_t sequence, bool *set_as_current_key_entry, uint32_t *replaced_block)
{
	return mock().actualCall("Directory::store_key").
		withUnsignedLongIntParameter("key", (unsigned long)key).
		withParameter("block", block).
		withUnsignedIntParameter("bytes", (unsigned)bytes).
		withUnsignedLongIntParameter("sequence", (unsigned long)sequence).
		returnIntValue();
}

int Directory::lookup_key(uint64_t key, uint32_t *block, size_t *bytes)
{
	return mock().actualCall("Directory::lookup_key").
		withUnsignedLongIntParameter("key", (unsigned long)key).
		returnIntValue();
}

int Directory::remove_key(uint64_t key)
{
	return mock().actualCall("Directory::remove_key").
		withUnsignedLongIntParameter("key", (unsigned long)key).
		returnIntValue();
}
