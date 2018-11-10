/*
 * BlockArray.h
 *
 *  Created on: Nov 5, 2018
 *      Author: jludwig
 */
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

struct kv_block_array;


int kv_block_array__init(struct kv_block_array **block_array, size_t raw_block_bytes);

void kv_block_array__cleanup(struct kv_block_array **block_array);


int kv_block_array__open(struct kv_block_array *block_array, const char *file_name, bool create);

int kv_block_array__get_file_block_count(const struct kv_block_array *block_array, uint32_t *total_blocks);

int kv_block_array__read_block(const struct kv_block_array *block_array, uint32_t block, uint8_t *block_data);

int kv_block_array__write_block(struct kv_block_array *block_array, uint32_t destination_block, const uint8_t *block_data);
