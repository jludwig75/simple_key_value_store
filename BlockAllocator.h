/*
 * BlockAllocator.h
 *
 *  Created on: Nov 5, 2018
 *      Author: jludwig
 */
#pragma once

#include <stdint.h>


class BlockAllocator {
public:
	BlockAllocator(uint32_t max_number_of_blocks);
	virtual ~BlockAllocator();

	uint32_t find_next_free_block(uint32_t starting_block);
	void free_block(uint32_t block);

	void mark_block_as_allocated(uint32_t block);

private:
	uint32_t _max_number_of_blocks;
};
