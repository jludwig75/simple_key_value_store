/*
 * AppendPoint.h
 *
 *  Created on: Nov 9, 2018
 *      Author: jludwig
 */
#pragma once

#include <stdint.h>

struct kv_block_allocator;

struct kv_append_point;

int kv_append_point__init(struct kv_append_point **append_point, struct kv_block_allocator *block_allocator);
void kv_append_point__cleanup(struct kv_append_point **append_point);

uint32_t kv_append_point__get_append_point(struct kv_append_point *append_point);
void kv_append_point__update_append_point(struct kv_append_point *append_point, uint32_t block, uint64_t sequence);
