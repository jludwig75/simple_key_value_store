/*
 * KvStore.h
 *
 *  Created on: Nov 5, 2018
 *      Author: jludwig
 */
#pragma once

#include <stdint.h>
#include <stdlib.h>


class Directory;
class Log;
class BlockAllocator;
class BlockArray;

class KvStore
{
public:
	KvStore(const char *file_name, bool create);
	virtual ~KvStore();

	int load();

	int format();

	int close();

	int get(uint64_t key, char **data, size_t *data_size) const;

	int set(uint64_t key, const char *data, size_t size);

	int del(uint64_t key);

private:
	BlockArray *_block_array;
	Directory *_directory;
	BlockAllocator *_block_allocator;
	Log *_log;
	char *_kv_store_file_name;
	bool _create;
};
