#include "BlockAllocator.h"

#include "CppUTestExt/MockSupport.h"


BlockAllocator::BlockAllocator(uint32_t number_of_blocks)
{
}

BlockAllocator::~BlockAllocator()
{

}

uint32_t BlockAllocator::find_next_free_block(uint32_t starting_block) const
{
	return (uint32_t)mock().actualCall("BlockAllocator::find_next_free_block").
							withParameter("starting_block", starting_block).
							returnUnsignedIntValue();
}

void BlockAllocator::free_block(uint32_t block)
{
	mock().actualCall("BlockAllocator::free_block").
			withParameter("block", block);
}

void BlockAllocator::mark_block_as_allocated(uint32_t block)
{
	mock().actualCall("BlockAllocator::mark_block_as_allocated").
			withParameter("block", block);
}

