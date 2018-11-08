/*
 * Log.cpp
 *
 *  Created on: Nov 5, 2018
 *      Author: jludwig
 */
#include "KvStore.h"

#include "BlockAllocator.h"
#include "BlockArray.h"
#include "Directory.h"
#include "kv_block.h"

#include "kv_store.h"

#include <assert.h>
#include <string.h>
#include <errno.h>


template<typename _T>
class DeleteOnExit
{
public:
	DeleteOnExit(_T * & data) : _data(data)
	{
	}
	~DeleteOnExit()
	{
		delete _data;
		_data = NULL;
	}
private:
	_T * &_data;
};


KvStore::KvStore() :
	_block_array(NULL),
	_directory(NULL),
	_block_allocator(NULL),
	_current_append_point(0),
	_current_sequence_number(0),
	_last_scanned_sequence_number(0)
{
	_block_allocator = new BlockAllocator(MAXKEYS + 1);
	_block_array = new BlockArray(sizeof(kv_block));
	_directory = new Directory;
}

KvStore::~KvStore()
{
	delete _block_allocator;
	delete _block_array;
	delete _directory;
}

int KvStore::open(const char *file_name, bool create)
{
	int ret = _block_array->open(file_name, create);
	if (ret != 0)
	{
		return ret;
	}

	if (!create)
	{
		return scan();
	}

	return 0;
}

void KvStore::close()
{
	_block_array->close();
}


int KvStore::scan()
{
	_last_scanned_sequence_number = 0;

	uint32_t total_blocks;
	int ret = _block_array->get_file_block_count(&total_blocks);
	if (ret != 0)
	{
		return ret;
	}

	kv_block block;
	uint32_t b;
	for(b = 0; b < total_blocks; b++)
	{
		ret = _block_array->read_block(b, (uint8_t *)&block);
		if (ret != 0)
		{
			return ret;
		}

		if (block.validate())
		{
			bool set_as_latest_entry_for_key;
			uint32_t replaced_block;
			ret = _directory->store_key(block.header.key_id, b, block.header.data_bytes, block.header.sequence, &set_as_latest_entry_for_key, &replaced_block);
			if (ret != 0)
			{
				return ret;
			}

			if (replaced_block != UINT32_MAX)
			{
				invalidate_block(replaced_block);
			}

			if (set_as_latest_entry_for_key && block.is_allocated())
			{
				_block_allocator->mark_block_as_allocated(b);
			}

			if (block.is_allocated())
			{
				update_append_point(b, block.header.sequence);
			}
		}
	}

	return 0;
}

int KvStore::get(uint64_t key, char *data, size_t data_buffer_bytes, size_t *bytes_read)
{
	uint32_t block;
	size_t stored_value_size;
	int ret = _directory->lookup_key(key, &block, &stored_value_size);
	if (ret != 0)
	{
		return ret;
	}
	if (data_buffer_bytes < stored_value_size)
	{
		return -EINVAL;
	}

	kv_block *block_data = new kv_block;
	DeleteOnExit<kv_block> on_exit(block_data);

	ret = _block_array->read_block(block, (uint8_t *)block_data);
	if (ret != 0)
	{
		return ret;
	}
	if (!block_data->validate())
	{
		return -EFAULT;
	}

	*bytes_read = stored_value_size;
	memcpy(data, block_data->data, stored_value_size);

	return 0;
}

int KvStore::set(uint64_t key, const char *value_data, size_t size)
{
	if (!value_data || size == 0 || size > MAXBLOB)
	{
		return -EINVAL;
	}

	uint64_t my_sequence = ++_current_sequence_number;
	kv_block *block_data = new kv_block(key, (uint32_t)size, value_data, my_sequence);
	DeleteOnExit<kv_block> on_exit(block_data);

	uint32_t destination_block = advance_append_point();
	if (destination_block == UINT32_MAX)
	{
		return -ENOSPC;
	}

	int ret = _block_array->write_block(destination_block, (const uint8_t *)block_data);
	if (ret != 0)
	{
		return ret;
	}

	bool set_as_latest_entry_for_key;
	uint32_t replaced_block;
	ret = _directory->store_key(key, destination_block, size, my_sequence, &set_as_latest_entry_for_key, &replaced_block);
	if (ret != 0)
	{
		return ret;
	}

	if (replaced_block != UINT32_MAX)
	{
		_block_allocator->free_block(replaced_block);
	}

	assert(set_as_latest_entry_for_key);
	_block_allocator->mark_block_as_allocated(destination_block);

	return 0;
}

int KvStore::del(uint64_t key)
{
	uint32_t old_block;
	size_t size;
	int ret = _directory->lookup_key(key, &old_block, &size);
	if (ret != 0)
	{
		return ret;
	}

	kv_block *block_data = new kv_block(key, 0, NULL, _current_sequence_number++);
	DeleteOnExit<kv_block> on_exit(block_data);

	uint32_t destination_block = advance_append_point();
	if (destination_block == UINT32_MAX)
	{
		return -ENOSPC;
	}

	ret = _block_array->write_block(destination_block, (const uint8_t *)block_data);
	if (ret != 0)
	{
		return ret;
	}

	ret = _directory->remove_key(key);
	if (ret != 0)
	{
		return ret;
	}

	invalidate_block(old_block);

	return 0;
}

void KvStore::invalidate_block(uint32_t block)
{
	_block_allocator->free_block(block);
}

uint32_t KvStore::advance_append_point()
{
	_current_append_point = _block_allocator->find_next_free_block(_current_append_point == UINT32_MAX ? 0 : _current_append_point);
	return _current_append_point;
}

void KvStore::update_append_point(uint32_t block, uint64_t sequence)
{
	if (sequence >= _last_scanned_sequence_number)	// >= to handle only block 0 written.
	{
		_current_append_point = block;
		_last_scanned_sequence_number = sequence;
	}
}
