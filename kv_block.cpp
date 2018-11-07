#include "kv_block.h"

#include <assert.h>
#include <string.h>


static const uint64_t kv_block_signature = 0xB1423CB4C5BCBC4ALL;


kv_block_header::kv_block_header() :
	signature(0), key_id(0), sequence(0), data_bytes(0)
{
}

kv_block_header::kv_block_header(uint64_t key_id, uint32_t data_bytes, uint64_t sequence) :
	signature(kv_block_signature), key_id(key_id), sequence(sequence), data_bytes(data_bytes)
{
}

bool kv_block_header::is_allocated() const
{
	return data_bytes > 0;
}

bool kv_block_header::validate() const
{
	return signature == kv_block_signature;
}



kv_block::kv_block()
{

}

kv_block::kv_block(uint64_t key_id, uint32_t data_bytes, const char *value_data, uint64_t sequence) : header(key_id, data_bytes, sequence)
{
	assert(data_bytes <= MAXBLOB);
	if (value_data)
	{
		memcpy(data, value_data, data_bytes);
	}
}

bool kv_block::is_allocated() const
{
	return header.is_allocated();
}

bool kv_block::validate() const
{
	return header.validate();
}
