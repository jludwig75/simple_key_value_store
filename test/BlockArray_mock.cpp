#include "BlockArray.h"

#include "CppUTestExt/MockSupport.h"


BlockArray::BlockArray(size_t raw_block_bytes) :
	_raw_block_bytes(raw_block_bytes)
{
}

BlockArray::~BlockArray()
{
}

int BlockArray::open(const char *file_name, bool create)
{
	return mock().actualCall("BlockArray::open").
			withStringParameter("file_name", file_name).
			withBoolParameter("create", create).
			returnIntValue();
}

void BlockArray::close()
{
	mock().actualCall("BlockArray::close");
}

int BlockArray::get_file_block_count(uint32_t *total_blocks) const
{
	return mock().actualCall("BlockArray::get_file_block_count").
					withOutputParameter("total_blocks", total_blocks).
					returnIntValue();
}

int BlockArray::read_block(uint32_t block, uint8_t *block_data) const
{
	return mock().actualCall("BlockArray::read_block").
			withParameter("block", block).
			withPointerParameter("block_data", block_data).
			returnIntValue();
}

int BlockArray::write_block(uint32_t destination_block, const uint8_t *block_data)
{
	return mock().actualCall("BlockArray::write_block").
			withParameter("destination_block", destination_block).
			withConstPointerParameter("block_data", block_data).
			returnIntValue();
}
