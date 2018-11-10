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

class Directory
{
public:
    Directory();
    virtual ~Directory();

    int store_key(uint64_t key, uint32_t block, size_t bytes, uint64_t sequence, bool *set_as_current_key_entry, uint32_t *replaced_block);
    int lookup_key(uint64_t key, uint32_t *block, size_t *bytes);
    int remove_key(uint64_t key);

private:
    directory_entry *find_entry_for_key(uint64_t key);

    void *_entries_root;
};
