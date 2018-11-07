/*
 * kv_block.h
 *
 *  Created on: Nov 7, 2018
 *      Author: jludwig
 */
#pragma once

#include "kv_store.h"


struct kv_block_header
{
	uint64_t signature;
	uint64_t key_id;
	uint64_t sequence;
	uint16_t data_bytes;

	kv_block_header();
	kv_block_header(uint64_t key_id, uint32_t data_bytes, uint64_t sequence);
	bool is_allocated() const;
	bool validate() const;
};

struct kv_block
{
	kv_block_header header;
	char data[MAXBLOB];

	kv_block();
	kv_block(uint64_t key_id, uint32_t data_bytes, const char *value_data, uint64_t sequence);
	bool is_allocated() const;
	bool validate() const;
};


