/*
 * Directory.cpp
 *
 *  Created on: Nov 5, 2018
 *      Author: jludwig
 */

#include "Directory.h"

#include <errno.h>
#include <assert.h>


struct directory_entry
{
	directory_entry() : is_allocated(false), key_id(0), data_block(UINT32_MAX), data_bytes(0)
	{
	}
	bool is_allocated;
	uint64_t key_id;
	uint32_t data_block;
	uint16_t data_bytes;
};

Directory::Directory(size_t max_number_of_entries) :
	_directory_entries(NULL), _max_directory_entries(max_number_of_entries), _number_of_actice_directory_entries(0)
{
	_directory_entries = new directory_entry[_max_directory_entries];
}

Directory::~Directory()
{
	delete [] _directory_entries;
}

int Directory::store_key(uint64_t key, uint32_t block, size_t bytes)
{
	if (_number_of_actice_directory_entries >= _max_directory_entries)
	{
		return -ENOSPC;
	}

	for(size_t i = 0; i < _max_directory_entries; i++)
	{
		if (!_directory_entries[i].is_allocated)
		{
			_directory_entries[i].key_id = key;
			_directory_entries[i].data_bytes = bytes;
			_directory_entries[i].data_block = block;
			_directory_entries[i].is_allocated = true;

			_number_of_actice_directory_entries++;
			return 0;
		}
	}

	assert(false && "This should not be possible");
	return -ENOSPC;
}

size_t Directory::find_entry_index_for_key(uint64_t key)
{
	for(size_t i = 0; i < _max_directory_entries; i++)
	{
		if (_directory_entries[i].is_allocated && _directory_entries[i].key_id == key)
		{
			return i;
		}
	}

	return _max_directory_entries;
}

int Directory::lookup_key(uint64_t key, uint32_t *block, size_t *bytes)
{
	size_t index = find_entry_index_for_key(key);
	if (index == _max_directory_entries)
	{
		return -ENOENT;
	}

	*block = _directory_entries[index].data_block;
	*bytes = _directory_entries[index].data_bytes;
	return 0;
}

int Directory::remove_key(uint64_t key)
{
	size_t index = find_entry_index_for_key(key);
	if (index == _max_directory_entries)
	{
		return -ENOENT;
	}

	_directory_entries[index].is_allocated = false;
	_number_of_actice_directory_entries--;
	return 0;
}
