/*
 * BlockManager.h
 *
 *  Created on: Nov 5, 2018
 *      Author: jludwig
 */
#pragma once

#include <stdint.h>
#include <stdlib.h>


class BlockArray;
class BlockAllocator;


class Log {
public:
	Log(BlockArray *block_array, BlockAllocator *block_allocator);
	virtual ~Log();

	static size_t get_raw_block_size();

	int read_block(uint32_t block, char *block_data, size_t bytes_to_read);
	int write_block(uint64_t key, const char *block_data, size_t size, uint32_t *block_written);
	int release_key(uint64_t key);

private:
	uint32_t advance_append_point();
	BlockArray *_block_array;
	BlockAllocator *_block_allocator;
	uint32_t _current_append_point;
	uint64_t _current_sequence_number;
};
