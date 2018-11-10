/*
 * AppendPoint.cpp
 *
 *  Created on: Nov 9, 2018
 *      Author: jludwig
 */
#include "AppendPoint.h"

#include "BlockAllocator.h"

AppendPoint::AppendPoint(BlockAllocator *block_allocator) :
	_block_allocator(block_allocator),
	_current_append_point(0),
	_last_scanned_sequence_number(0)
{
}

AppendPoint::~AppendPoint()
{
}

uint32_t AppendPoint::get_append_point()
{
	if (_current_append_point == UINT32_MAX)
	{
		// If we ran out of space before, start back at 0.
		_current_append_point = 0;
	}
	_current_append_point = _block_allocator->find_next_free_block(_current_append_point);
	return _current_append_point;
}

void AppendPoint::update_append_point(uint32_t block, uint64_t sequence)
{
	if (sequence >= _last_scanned_sequence_number)	// >= to handle only block 0 written.
	{
		_current_append_point = block;
		_last_scanned_sequence_number = sequence;
	}
}
