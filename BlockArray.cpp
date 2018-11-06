/*
 * BlockArray.cpp
 *
 *  Created on: Nov 5, 2018
 *      Author: jludwig
 */

#include "BlockArray.h"

BlockArray::BlockArray(size_t raw_block_bytes) :
	_raw_block_bytes(raw_block_bytes)
{
}

BlockArray::~BlockArray()
{
}

int BlockArray::open(const char *file_name, bool create)
{

}
