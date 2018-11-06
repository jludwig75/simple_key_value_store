/*
 * Log.cpp
 *
 *  Created on: Nov 5, 2018
 *      Author: jludwig
 */
#include "Log.h"
#include "BlockAllocator.h"
#include "BlockArray.h"

#include "kv_store.h"

#include <assert.h>
#include <string.h>
#include <errno.h>


const uint64_t kv_block_signature = 0xB1423CB4C5BCBC4ALL;


struct kv_block_header
{
	kv_block_header() :
		signature(0), key_id(0), data_bytes(0), sequence(0)
	{
	}
	kv_block_header(uint64_t key_id, uint32_t data_bytes, uint64_t sequence) :
		signature(kv_block_signature), key_id(key_id), data_bytes(data_bytes), sequence(0)
	{
	}
	bool is_allocated() const
	{
		return data_bytes > 0;
	}
	bool validate() const
	{
		return signature == kv_block_signature;
	}
	uint64_t signature;
	uint64_t key_id;
	uint64_t sequence;
	uint16_t data_bytes;
};

struct kv_block
{
	kv_block()
	{

	}
	kv_block(uint64_t key_id, uint32_t data_bytes, const char *value_data, uint64_t sequence) : header(key_id, data_bytes, sequence)
	{
		assert(data_bytes <= MAXBLOB);
		if (value_data)
		{
			memcpy(data, value_data, data_bytes);
		}
	}
	bool is_allocated() const
	{
		return header.is_allocated();
	}
	bool validate() const
	{
		return header.validate();
	}
	kv_block_header header;
	char data[MAXBLOB];
};

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


Log::Log(BlockArray *block_array, BlockAllocator *block_allocator) :
	_block_array(block_array),
	_block_allocator(block_allocator),
	_current_append_point(UINT32_MAX),
	_current_sequence_number(0)
{
}

Log::~Log()
{
}

size_t Log::get_raw_block_size()
{
	return sizeof(kv_block);
}

int Log::read_block(uint32_t block, char *data, size_t bytes_to_read)
{
	kv_block *block_data = new kv_block;
	DeleteOnExit<kv_block> on_exit(block_data);

	int ret = _block_array->read_block(block, (uint8_t *)block_data);
	if (ret != 0)
	{
		return ret;
	}

	if (!block_data->validate())
	{
		return -EFAULT;
	}

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

int Log::release_key(uint64_t key)
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

uint32_t Log::advance_append_point()
{
	_current_append_point = _block_allocator->find_next_free_block(_current_append_point);
	return _current_append_point;
}
