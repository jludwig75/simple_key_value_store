/*
 * Directory.h
 *
 *  Created on: Nov 5, 2018
 *      Author: jludwig
 */
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

struct kv_directory;


int kv_directory__init(struct kv_directory **directory);

void kv_directory__cleanup(struct kv_directory **directory);

int kv_directory__store_key(struct kv_directory *directory, uint64_t key, uint32_t block, size_t bytes, uint64_t sequence, bool *set_as_current_key_entry, uint32_t *replaced_block);
int kv_directory__lookup_key(struct kv_directory *directory, uint64_t key, uint32_t *block, size_t *bytes);
int kv_directory__remove_key(struct kv_directory *directory, uint64_t key);
