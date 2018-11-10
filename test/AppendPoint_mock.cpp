#include "AppendPoint.h"

#include "CppUTestExt/MockSupport.h"


AppendPoint::AppendPoint(BlockAllocator *block_allocator) : 
	_block_allocator(block_allocator)
{
}

AppendPoint::~AppendPoint()
{
}

uint32_t AppendPoint::get_append_point()
{
	return mock().actualCall("AppendPoint::get_append_point").
					returnUnsignedIntValue();
}

void AppendPoint::update_append_point(uint32_t block, uint64_t sequence)
{
	mock().actualCall("AppendPoint::get_append_point").
		withParameter("block", block).
		withUnsignedLongIntParameter("sequence", (unsigned long)sequence);
}

