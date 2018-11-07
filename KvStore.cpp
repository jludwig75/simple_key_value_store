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

#include <string.h>


KvStore::KvStore(const char *file_name, bool create) :
	_block_array(NULL),
	_directory(NULL),
	_block_allocator(NULL),
	_log(NULL),
	_kv_store_file_name(NULL),
	_create(create)
{
	_block_array = new BlockArray(Log::get_raw_block_size());
	_directory = new Directory(MAXKEYS);
	_block_allocator = new BlockAllocator(MAXBLOB);
	_log = new Log(_block_array, _block_allocator);
	_kv_store_file_name = new char[strlen(file_name) + 1];
	strcpy(_kv_store_file_name, file_name);
}

KvStore::~KvStore()
{
	delete _block_array;
	delete _directory;
	delete _block_allocator;
	delete _log;
	delete _kv_store_file_name;
}

int KvStore::load()
{
	return 0;
}

int KvStore::format()
{
	int ret = _block_array->open(_kv_store_file_name, _create);
	if (ret != 0)
	{
		return ret;
	}

	return _log->format();
}

int KvStore::close()
{
	return 0;
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
	*data = new char[bytes];
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
	if (ret == 0)
	{
		// Release the key. Maybe save off the block to free it later.
		_block_allocator->free_block(block);
	}

	uint32_t new_block;
	ret = _log->write_block(key, data, size, &new_block);
	if (ret != 0)
	{
		return ret;
	}

	ret = _directory->store_key(key, new_block, size);
	if (ret != 0)
	{
		return ret;
	}

	_block_allocator->mark_block_as_allocated(new_block);
	return 0;
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
