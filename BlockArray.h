/*
 * BlockArray.h
 *
 *  Created on: Nov 5, 2018
 *      Author: jludwig
 */
#pragma once

#include <stdint.h>
#include <stdlib.h>


class BlockArray {
public:
	BlockArray(size_t raw_block_bytes);
	virtual ~BlockArray();

	int open(const char *file_name, bool create);

	int read_block(uint32_t block, uint8_t *block_data);

	int write_block(uint32_t destination_block, const uint8_t *block_data);

	int truncate();

private:
	const size_t _raw_block_bytes;
	int _fd;
};
