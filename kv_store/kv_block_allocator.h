/*
 * BlockAllocator.h
 *
 *  Created on: Nov 5, 2018
 *      Author: jludwig
 */
#pragma once

#include <stdint.h>
#include <stdlib.h>


struct kv_block_allocator;

int kv_block_allocator__init(struct kv_block_allocator **block_allocator, uint32_t number_of_blocks);
void kv_block_allocator__cleanup(struct kv_block_allocator **block_allocator);

uint32_t kv_block_allocator__find_next_free_block(const struct kv_block_allocator *block_allocator, uint32_t starting_block);
void kv_block_allocator__free_block(struct kv_block_allocator *block_allocator, uint32_t block);
void kv_block_allocator__mark_block_as_allocated(struct kv_block_allocator *block_allocator, uint32_t block);
