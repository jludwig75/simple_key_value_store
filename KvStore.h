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
class Directory;


class KvStore {
public:
	KvStore();
	virtual ~KvStore();

	int open(const char *file_name, bool create);
	void close();

	int get(uint64_t key, char *block_data, size_t data_buffer_bytes, size_t *bytes_read);
	int set(uint64_t key, const char *block_data, size_t size);
	int del(uint64_t key);

private:
	int scan();
	void invalidate_block(uint32_t block);
	uint32_t advance_append_point();
	void update_append_point(uint32_t block, uint64_t sequence);

	BlockArray *_block_array;
	Directory *_directory;
	BlockAllocator *_block_allocator;

	uint32_t _current_append_point;
	uint64_t _current_sequence_number;
	uint64_t _last_scanned_sequence_number;
};
