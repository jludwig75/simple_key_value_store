/*
 * Directory.h
 *
 *  Created on: Nov 5, 2018
 *      Author: jludwig
 */
#pragma once

#include <stdint.h>
#include <stdlib.h>


struct directory_entry;

class Directory {
public:
	Directory(size_t max_number_of_entries);
	virtual ~Directory();

	int store_key(uint64_t key, uint32_t block, size_t bytes);
	int lookup_key(uint64_t key, uint32_t *block, size_t *bytes);
	int remove_key(uint64_t key);

private:
	directory_entry *find_entry_for_key(uint64_t key);

	void *_entries_root;
	const size_t _max_directory_entries;
	size_t _number_of_actice_directory_entries;
};
