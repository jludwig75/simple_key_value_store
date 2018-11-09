/*
 * Directory.cpp
 *
 *  Created on: Nov 5, 2018
 *      Author: jludwig
 */

#include "Directory.h"

#include <errno.h>
#include <assert.h>
#include <search.h>
#include <stdio.h>


struct directory_entry
{
	directory_entry(uint64_t key_id, uint64_t sequence, uint32_t data_block, uint16_t data_bytes) :
		key_id(key_id),
		sequence(sequence),
		data_block(data_block),
		data_bytes(data_bytes)
	{
	}
	uint64_t key_id;
	uint64_t sequence;
	uint32_t data_block;
	uint16_t data_bytes;
	bool is_allocated() const
	{
		return data_bytes > 0;
	}
};


static int dir_entry_compare(const void *pa, const void *pb)
{
	const directory_entry *entry_a = (const directory_entry *)pa;
	const directory_entry *entry_b = (const directory_entry *)pb;

	if (entry_a->key_id < entry_b->key_id)
	{
		return -1;
	}
	if (entry_a->key_id > entry_b->key_id)
	{
		return 1;
	}
	return 0;
}

static void dir_entry_free(void *node)
{
	directory_entry *entry = (directory_entry *)node;
	delete entry;
}

Directory::Directory() :
	_entries_root(NULL)
{
}

Directory::~Directory()
{
	tdestroy(_entries_root, dir_entry_free);
}

int Directory::store_key(uint64_t key, uint32_t block, size_t bytes, uint64_t sequence, bool *set_as_current_key_entry, uint32_t *replaced_block)
{
	*set_as_current_key_entry = false;
	*replaced_block = UINT32_MAX;

	directory_entry *entry = find_entry_for_key(key);
	if (entry)
	{
		// This should never happen.
		assert(sequence != entry->sequence);
		if (sequence > entry->sequence)
		{
			*replaced_block = entry->data_block;
			entry->sequence = sequence;
			entry->data_block = block;
			entry->data_bytes = bytes;
			*set_as_current_key_entry = true;
		}
		return 0;
	}

	directory_entry *new_entry = new directory_entry(key, sequence, block, bytes);
	*set_as_current_key_entry = true;

	directory_entry **new_node = (directory_entry **)tsearch(new_entry, &_entries_root, dir_entry_compare);
	if (!new_node)
	{
		return -ENOMEM;
	}
	if (*new_node != new_entry)
	{
		// Our allocated entry was not stored.
		// Delete it here.
		delete new_entry;
	}
	return 0;
}

directory_entry *Directory::find_entry_for_key(uint64_t key)
{
	directory_entry entry_key(key, 0, 0, 0);

	directory_entry **node = (directory_entry **)tfind(&entry_key, &_entries_root, dir_entry_compare);
	if (!node)
	{
		return NULL;
	}

	return *node;
}

int Directory::lookup_key(uint64_t key, uint32_t *block, size_t *bytes)
{
	directory_entry *entry = find_entry_for_key(key);
	if (!entry || !entry->is_allocated())
	{
		return -ENOENT;
	}

	*block = entry->data_block;
	*bytes = entry->data_bytes;
	return 0;
}

int Directory::remove_key(uint64_t key)
{
	directory_entry *entry = find_entry_for_key(key);
	if (!entry || !entry->is_allocated())
	{
		return -ENOENT;
	}

	if (!tdelete(entry, &_entries_root, dir_entry_compare))
	{
		return -EFAULT;
	}
	delete entry;
	return 0;
}
