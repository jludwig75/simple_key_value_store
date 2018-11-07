/*
 * KvStore.cpp
 *
 *  Created on: Nov 5, 2018
 *      Author: jludwig
 */

#include "KvStore.h"

#include "Directory.h"
#include "Log.h"
#include "BlockArray.h"

#include "kv_store.h"

#include <string.h>


KvStore::KvStore(const char *file_name, bool create) :
	_block_array(NULL),
	_directory(NULL),
	_log(NULL),
	_kv_store_file_name(NULL),
	_create(create)
{
	_block_array = new BlockArray(Log::get_raw_block_size());
	_directory = new Directory(MAXKEYS);
	_log = new Log(_block_array);
	_kv_store_file_name = new char[strlen(file_name) + 1];
	strcpy(_kv_store_file_name, file_name);
}

KvStore::~KvStore()
{
	delete _block_array;
	delete _directory;
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
	// lookup the key in case it is already stored.
	uint32_t old_block;
	size_t bytes;
	bool invalidate_old_block = false;
	int ret = _directory->lookup_key(key, &old_block, &bytes);
	// If the key was already stored, we will need to invalidate
	// the old block after we replace it.
	bool invalidate_old_block = (ret == 0);

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

	if (invalidate_old_block)
	{
		_log->invalidate_block(old_block);
	}

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

	ret = _log->mark_key_as_erased(key);
	if (ret != 0)
	{
		return ret;
	}

	return _directory->remove_key(key);
}
