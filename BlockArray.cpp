/*
 * BlockArray.cpp
 *
 *  Created on: Nov 5, 2018
 *      Author: jludwig
 */

#include "BlockArray.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
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
	if (_fd != -1)
	{
		close(_fd);
	}
}

int BlockArray::open(const char *file_name, bool create)
{
	_fd = ::open(file_name, O_RDWR | (create ? O_CREAT : 0), S_IRWXU);
	if (_fd == -1)
	{
		return errno;
	}
	return 0;
}

int BlockArray::read_block(uint32_t block, uint8_t *block_data)
{
	if (lseek(_fd, block * _raw_block_bytes, SEEK_SET) == -1)
	{
		return errno;
	}

	int ret = read(_fd, block_data, _raw_block_bytes);
	if (ret == -1)
	{
		return errno;
	}
	if ((size_t)ret != _raw_block_bytes)
	{
		return -EIO;
	}

	return 0;
}

int BlockArray::write_block(uint32_t destination_block, const uint8_t *block_data)
{
	if (lseek(_fd, destination_block * _raw_block_bytes, SEEK_SET) == -1)
	{
		return errno;
	}

	int ret = write(_fd, block_data, _raw_block_bytes);
	if (ret == -1)
	{
		return errno;
	}
	if ((size_t)ret != _raw_block_bytes)
	{
		return -EIO;
	}

	return 0;
}

int BlockArray::truncate()
{
	if (ftruncate(_fd, 0) != 0)
	{
		return errno;
	}

	return 0;
}
