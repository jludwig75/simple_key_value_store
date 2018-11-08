/*
 * Log.cpp
 *
 *  Created on: Nov 5, 2018
 *      Author: jludwig
 */
#include "Log.h"
#include "BlockAllocator.h"
#include "BlockArray.h"
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


Log::Log(BlockArray *block_array) :
	_block_array(block_array),
	_block_allocator(NULL),
	_current_append_point(0),
	_current_sequence_number(0)
{
	_block_allocator = new BlockAllocator(MAXKEYS);
}

Log::~Log()
{
	delete _block_allocator;
}

size_t Log::get_raw_block_size()
{
	return sizeof(kv_block);
}

int Log::format()
{
	return _block_array->truncate();
}

int Log::read_block(uint32_t block, char *data, size_t bytes_to_read)
{
	kv_block *block_data = new kv_block;
	DeleteOnExit<kv_block> on_exit(block_data);

	if (bytes_to_read > sizeof(block_data->data))
	{
		return -EINVAL;
	}

	int ret = _block_array->read_block(block, (uint8_t *)block_data);
	if (ret != 0)
	{
		return ret;
	}

	if (!block_data->validate())
	{
		return -EFAULT;
	}

	memcpy(data, block_data->data, bytes_to_read);

	return 0;
}

int Log::write_block(uint64_t key, const char *value_data, size_t size, uint32_t *block_written)
{
	kv_block *block_data = new kv_block(key, (uint32_t)size, value_data, _current_sequence_number++);
	DeleteOnExit<kv_block> on_exit(block_data);

	uint32_t destination_block = advance_append_point();
	if (destination_block == UINT32_MAX)
	{
		return -ENOSPC;
	}
	*block_written = destination_block;

	return _block_array->write_block(destination_block, (const uint8_t *)block_data);
}

int Log::mark_key_as_erased(uint64_t key)
{
	kv_block *block_data = new kv_block(key, 0, NULL, _current_sequence_number++);
	DeleteOnExit<kv_block> on_exit(block_data);

	uint32_t destination_block = advance_append_point();
	if (destination_block == UINT32_MAX)
	{
		return -ENOSPC;
	}

	return _block_array->write_block(destination_block, (const uint8_t *)block_data);
}

void Log::invalidate_block(uint32_t block)
{
	_block_allocator->free_block(block);
}

uint32_t Log::advance_append_point()
{
	_current_append_point = _block_allocator->find_next_free_block(_current_append_point);
	return _current_append_point;
}
