#include "kv_block.h"

#include "CppUTestExt/MockSupport.h"


kv_block_header::kv_block_header()
{
}

kv_block_header::kv_block_header(uint64_t key_id, uint32_t data_bytes, uint64_t sequence)
{
}

bool kv_block_header::is_allocated() const
{
	return mock().actualCall("kv_block_header::is_allocated").returnBoolValue();
}

bool kv_block_header::validate() const
{
	return mock().actualCall("kv_block_header::validate").returnBoolValue();
}



kv_block::kv_block()
{

}

kv_block::kv_block(uint64_t key_id, uint32_t data_bytes, const char *value_data, uint64_t sequence)
{
}

bool kv_block::is_allocated() const
{
	return mock().actualCall("kv_block::is_allocated").returnBoolValue();
}

bool kv_block::validate() const
{
	return mock().actualCall("kv_block::validate").returnBoolValue();
}

