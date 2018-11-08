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
	directory_entry(uint64_t key_id, uint32_t data_block, uint16_t data_bytes) :
		key_id(key_id),
		data_block(data_block),
		data_bytes(data_bytes)
	{
	}
	uint64_t key_id;
	uint32_t data_block;
	uint16_t data_bytes;
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

Directory::Directory(size_t max_number_of_entries) :
	_entries_root(NULL), _max_directory_entries(max_number_of_entries), _number_of_actice_directory_entries(0)
{
}

Directory::~Directory()
{
	tdestroy(_entries_root, dir_entry_free);
}

int Directory::store_key(uint64_t key, uint32_t block, size_t bytes)
{
	if (_number_of_actice_directory_entries >= _max_directory_entries)
	{
		return -ENOSPC;
	}

	directory_entry *new_entry = new directory_entry(key, block, bytes);

	void *new_node = tsearch(new_entry, &_entries_root, dir_entry_compare);
	if (!new_node)
	{
		return -ENOMEM;
	}
	_number_of_actice_directory_entries++;
	return 0;
}

directory_entry *Directory::find_entry_for_key(uint64_t key)
{
	directory_entry entry_key(key, 0, 0);

	directory_entry **node = (directory_entry **)tfind(&entry_key, &_entries_root, dir_entry_compare);
	if (!node)
	{
		return NULL;
	}

	return *node;
}

int Directory::lookup_key(uint64_t key, uint32_t *block, size_t *bytes)
{
	directory_entry *enrty = find_entry_for_key(key);
	if (!enrty)
	{
		return -ENOENT;
	}

	*block = enrty->data_block;
	*bytes = enrty->data_bytes;
	return 0;
}

int Directory::remove_key(uint64_t key)
{
	directory_entry *entry = find_entry_for_key(key);
	if (!entry)
	{
		return -ENOENT;
	}

	if (!tdelete(entry, &_entries_root, dir_entry_compare))
	{
		return -EFAULT;
	}
	delete entry;
	_number_of_actice_directory_entries--;
	return 0;
}
