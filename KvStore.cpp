/*
 * KvStore.cpp
 *
 *  Created on: Nov 5, 2018
 *      Author: jludwig
 */

#include "KvStore.h"

#include "Directory.h"
#include "Log.h"
#include "BlockAllocator.h"
#include "BlockArray.h"

#include "kv_store.h"


KvStore::KvStore(const char *file_name, bool create) :
	_directory(NULL), _log(NULL), _block_allocator(NULL)
{
	_block_array = new BlockArray(Log::get_raw_block_size());
	_directory = new Directory(MAXKEYS);
	_block_allocator = new BlockAllocator(MAXBLOB);
	_log = new Log(_block_array, _block_allocator);
}

KvStore::~KvStore()
{
	delete _block_array;
	delete _directory;
	delete _block_allocator;
	delete _log;
}

int KvStore::get(uint64_t key, char **data, size_t *data_size) const
{
	// lookup the key
	uint32_t block;
	size_t bytes;
	int ret = _directory->lookup_key(key, &block, &bytes);
	if (ret != 0)
	{
		return ret;
	}

	// Allocate a buffer;
	*data = new char[data_size];
	*data_size = bytes;

	// read the block
	return _log->read_block(block, *data, bytes);
}

int KvStore::set(uint64_t key, const char *data, size_t size)
{
	// lookup the key
	uint32_t block;
	size_t bytes;
	int ret = _directory->lookup_key(key, &block, &bytes);
	if (ret != 0)
	{
		// Release the key. Maybe save off the block to free it later.
	}

	uint32_t block;
	int ret = _log->write_block(key, data, size, &block);
	if (ret != 0)
	{
		return ret;
	}

	ret = _directory->store_key(key, block, size);
	if (ret != 0)
	{
		return ret;
	}

	_block_allocator->mark_block_as_allocated(block);
}

int KvStore::del(uint64_t key)
{
	// lookup the key
	uint32_t block;
	size_t bytes;
	int ret = _directory->lookup_key(key, &block, &bytes);
	if (ret != 0)
	{
		return ret;
	}

	ret = _log->release_key(key);
	if (ret != 0)
	{
		return ret;
	}

	return _directory->remove_key(key);
}
