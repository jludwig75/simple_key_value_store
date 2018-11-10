/*
* BlockArray.cpp
*
*  Created on: Nov 5, 2018
*      Author: jludwig
*/

#include "BlockArray.h"

#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
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
	close();
}

int BlockArray::open(const char *file_name, bool create)
{
	_fd = _open(file_name, O_RDWR | O_BINARY | (create ? (O_CREAT | O_TRUNC) : 0), _S_IREAD | _S_IWRITE);
	if (_fd == -1)
	{
		return errno;
	}
	return 0;
}

void BlockArray::close()
{
	if (_fd != -1)
	{
		::close(_fd);
		_fd = -1;
	}
}


int BlockArray::get_file_block_count(uint32_t *total_blocks) const
{
	if (_fd == -1)
	{
		return -EBADF;
	}

	off_t off = lseek(_fd, 0, SEEK_END);

	if (off == (off_t)-1)
	{
		return errno;
	}

	*total_blocks = (uint32_t)(off / _raw_block_bytes);
	return 0;
}


int BlockArray::read_block(uint32_t block, uint8_t *block_data) const
{
	if (_fd == -1)
	{
		return -EBADF;
	}

	if (lseek(_fd, (long)(block * _raw_block_bytes), SEEK_SET) == -1)
	{
		return errno;
	}

	int ret = read(_fd, block_data, (unsigned)_raw_block_bytes);
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
	if (_fd == -1)
	{
		return -EBADF;
	}

	if (lseek(_fd, (long)(destination_block * _raw_block_bytes), SEEK_SET) == -1)
	{
		return errno;
	}

	int ret = write(_fd, block_data, (unsigned)_raw_block_bytes);
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
