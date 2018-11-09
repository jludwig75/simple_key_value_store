/*
 * AppendPoint.h
 *
 *  Created on: Nov 9, 2018
 *      Author: jludwig
 */
#pragma once

#include <stdint.h>


class BlockAllocator;

class AppendPoint {
public:
	AppendPoint(BlockAllocator *block_allocator);
	virtual ~AppendPoint();

	uint32_t get_append_point();
	void update_append_point(uint32_t block, uint64_t sequence);

private:
	BlockAllocator *_block_allocator;
	uint32_t _current_append_point;
	uint64_t _last_scanned_sequence_number;
};
