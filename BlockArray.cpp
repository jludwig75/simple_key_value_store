/*
 * BlockArray.cpp
 *
 *  Created on: Nov 5, 2018
 *      Author: jludwig
 */

#include "BlockArray.h"

#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>


BlockArray::BlockArray(size_t raw_block_bytes) :
	_raw_block_bytes(raw_block_bytes),
	_fd(-1)
{
}

BlockArray::~BlockArray()
{
}

int BlockArray::open(const char *file_name, bool create)
{
	_fd = _open(file_name, O_RDWR | O_BINARY | (create ? O_CREAT : 0));
	if (_fd == -1)
	{
		return errno;
	}
	return 0;
}

int BlockArray::read_block(uint32_t block, uint8_t *block_data)
{
	if (_lseek(_fd, block * _raw_block_bytes, SEEK_SET) == -1)
	{
		return errno;
	}

	int ret = _read(_fd, block_data, _raw_block_bytes);
	if (ret == -1)
	{
		return errno;
	}
	if (ret != _raw_block_bytes)
	{
		return -EIO;
	}

	return 0;
}

int BlockArray::write_block(uint32_t destination_block, const uint8_t *block_data)
{
	if (_lseek(_fd, destination_block * _raw_block_bytes, SEEK_SET) == -1)
	{
		return errno;
	}

	int ret = _write(_fd, block_data, _raw_block_bytes);
	if (ret == -1)
	{
		return errno;
	}
	if (ret != _raw_block_bytes)
	{
		return -EIO;
	}

	return 0;
}

int BlockArray::truncate()
{
	return 0;
}
