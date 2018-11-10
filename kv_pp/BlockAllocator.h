/*
 * BlockAllocator.h
 *
 *  Created on: Nov 5, 2018
 *      Author: jludwig
 */
#pragma once

#include <stdint.h>
#include <stdlib.h>


class BlockAllocator {
public:
	BlockAllocator(uint32_t number_of_blocks);
	virtual ~BlockAllocator();

	uint32_t find_next_free_block(uint32_t starting_block) const;
	void free_block(uint32_t block);
	void mark_block_as_allocated(uint32_t block);

private:
	bool is_set(uint32_t block) const;
	void set(uint32_t block);
	void clear(uint32_t block);
	size_t block_to_byte(uint32_t block) const;
	size_t block_to_bit(uint32_t block) const;
	uint32_t _number_of_blocks;
	uint8_t *_bits;
};
