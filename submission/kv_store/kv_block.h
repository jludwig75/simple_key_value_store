/*
 * kv_block.h
 *
 *  Created on: Nov 7, 2018
 *      Author: jludwig
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

struct kv_block_header
{
    uint64_t signature;
    uint64_t key_id;
    uint64_t sequence;
    uint16_t data_bytes;
	uint16_t pad1;
	uint32_t pad2;
};

struct kv_block
{
    struct kv_block_header header;
    char data[4096];
};

void kv_block__init_empty(struct kv_block *bv_block);
void kv_block__init(struct kv_block *bv_block, uint64_t key_id, uint32_t data_bytes, const char *value_data, uint64_t sequence);
bool kv_block__is_allocated(const struct kv_block *kv_block);
bool kv_block__validate(const struct kv_block *kv_block);


