/*
 * BlockAllocator.cpp
 *
 *  Created on: Nov 5, 2018
 *      Author: jludwig
 */
#include "BlockAllocator.h"

#include <assert.h>
#include <errno.h>
#include <string.h>

BlockAllocator::BlockAllocator(uint32_t number_of_blocks) :
	_number_of_blocks(number_of_blocks)
{
	size_t bit_array_bytes = (number_of_blocks + 7) / 8;
	_bits = new uint8_t[bit_array_bytes];
	memset(_bits, 0, bit_array_bytes);
}

BlockAllocator::~BlockAllocator()
{
	delete[] _bits;
}

uint32_t BlockAllocator::find_next_free_block(uint32_t starting_block)
{
	assert(starting_block < _number_of_blocks);
	if (starting_block >= _number_of_blocks)
	{
		return UINT32_MAX;
	}

	for (uint32_t b = starting_block; b < _number_of_blocks; b++)
	{
		if (!is_set(b))
		{
			return b;
		}
	}
	if (starting_block > 0)
	{
		for (uint32_t b = 0; b < starting_block; b++)
		{
			if (!is_set(b))
			{
				return b;
			}
		}
	}
	return UINT32_MAX;
}

void BlockAllocator::free_block(uint32_t block)
{
	assert(block < _number_of_blocks);

	clear(block);
}

void BlockAllocator::mark_block_as_allocated(uint32_t block)
{
	assert(block < _number_of_blocks);

	set(block);
}

bool BlockAllocator::is_set(uint32_t block) const
{
	return ((1 << block_to_bit(block)) & _bits[block_to_byte(block)]) != 0;
}

void BlockAllocator::set(uint32_t block) const
{
	_bits[block_to_byte(block)] |= (1 << block_to_bit(block));
}

void BlockAllocator::clear(uint32_t block) const
{
	_bits[block_to_byte(block)] &= ~(1 << block_to_bit(block));
}


size_t BlockAllocator::block_to_byte(uint32_t block) const
{
	assert(block < _number_of_blocks);

	return block / 8;
}

size_t BlockAllocator::block_to_bit(uint32_t block) const
{
	assert(block < _number_of_blocks);

	return block % 8;
}
