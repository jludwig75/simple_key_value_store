/*
 * Log.cpp
 *
 *  Created on: Nov 5, 2018
 *      Author: jludwig
 */
#include "kv_store.h"

#include "kv_block_allocator.h"
#include "kv_block_array.h"
#include "kv_directory.h"
#include "kv_append_point.h"

#include "kv_block.h"

#include <assert.h>
#include <string.h>
#include <errno.h>


struct kvstor
{
    struct kv_block_array *block_array;
    struct kv_directory *directory;
    struct kv_block_allocator *block_allocator;
    struct kv_append_point *append_point;

    uint32_t current_append_point;
    uint64_t current_sequence_number;
    uint64_t last_scanned_sequence_number;
};

static int kv_store__init(struct kvstor **store)
{
    *store = (struct kvstor *)malloc(sizeof(struct kvstor));
    if (!*store)
    {
        return -ENOMEM;
    }

    (*store)->current_append_point = 0;
    (*store)->current_sequence_number = 0;
    (*store)->last_scanned_sequence_number = 0;

    int ret = kv_block_allocator__init(&(*store)->block_allocator, MAXKEYS + 1);
    if (ret != 0)
    {
        free(*store);
        return ret;
    }

    ret = kv_block_array__init(&(*store)->block_array, sizeof(struct kv_block));
    if (ret != 0)
    {
        free(*store);
        return ret;
    }

    ret = kv_directory__init(&(*store)->directory);
    if (ret != 0)
    {
        free(*store);
        return ret;
    }

    ret = kv_append_point__init(&(*store)->append_point, (*store)->block_allocator);
    if (ret != 0)
    {
        free(*store);
        return ret;
    }

    return 0;
}

int kv_store__scan(struct kvstor *store)
{
    store->last_scanned_sequence_number = 0;

    uint32_t total_blocks;
    int ret = kv_block_array__get_file_block_count(store->block_array, &total_blocks);
    if (ret != 0)
    {
        return ret;
    }

    struct kv_block block;
    kv_block__init_empty(&block);
    uint32_t b;
    for (b = 0; b < total_blocks; b++)
    {
        ret = kv_block_array__read_block(store->block_array, b, (uint8_t *)&block);
        if (ret != 0)
        {
            return ret;
        }

        if (kv_block__validate(&block))
        {
            bool set_as_latest_entry_for_key;
            uint32_t replaced_block;
            ret = kv_directory__store_key(store->directory, block.header.key_id, b, block.header.data_bytes, block.header.sequence, &set_as_latest_entry_for_key, &replaced_block);
            if (ret != 0)
            {
                return ret;
            }

            if (replaced_block != UINT32_MAX)
            {
                kv_block_allocator__free_block(store->block_allocator, replaced_block);
            }

            if (set_as_latest_entry_for_key && kv_block__is_allocated(&block))
            {
                kv_block_allocator__mark_block_as_allocated(store->block_allocator, b);
            }

            kv_append_point__update_append_point(store->append_point, b, block.header.sequence);
        }
    }

    return 0;
}

int kv_open(struct kvstor **store, bool create, int argc, char **argv)
{
    if (argc < 2)
    {
        return -EINVAL;
    }

    int ret = kv_store__init(store);
    if (ret != 0)
    {
        return ret;
    }

    ret = kv_block_array__open((*store)->block_array, argv[1], create);
    if (ret != 0)
    {
        return ret;
    }

    if (!create)
    {
        return kv_store__scan(*store);
    }

    return 0;
}

void kv_close(struct kvstor *store)
{
    kv_block_allocator__cleanup(&store->block_allocator);
    kv_block_array__cleanup(&store->block_array);
    kv_directory__cleanup(&store->directory);
    kv_append_point__cleanup(&store->append_point);

    free(store);
}


int kv_get(struct kvstor *store, const struct key *k, struct value *v)
{
    uint32_t block;
    size_t stored_value_size;
    int ret = kv_directory__lookup_key(store->directory, k->id, &block, &stored_value_size);
    if (ret != 0)
    {
        return ret;
    }
    if (v->size < stored_value_size)
    {
        return -EINVAL;
    }

    /// @todo Allocate on heap rather than stack
    struct kv_block block_data;
    kv_block__init_empty(&block_data);

    ret = kv_block_array__read_block(store->block_array, block, (uint8_t *)&block_data);
    if (ret != 0)
    {
        return ret;
    }
    if (!kv_block__validate(&block_data))
    {
        return -EFAULT;
    }

    v->size = stored_value_size;
    memcpy(v->data, block_data.data, stored_value_size);

    return 0;
}

int kv_set(struct kvstor *store, const struct key *k, const struct value *v)
{
    if (v->size == 0 || v->size > MAXBLOB)
    {
        return -EINVAL;
    }

    uint64_t my_sequence = ++store->current_sequence_number;
    /// @todo Allocate on heap rather than stack
    struct kv_block block_data;
    kv_block__init(&block_data, k->id, (uint32_t)v->size, v->data, my_sequence);

    uint32_t destination_block = kv_append_point__get_append_point(store->append_point);
    if (destination_block == UINT32_MAX)
    {
        return -ENOSPC;
    }

    int ret = kv_block_array__write_block(store->block_array, destination_block, (const uint8_t *)&block_data);
    if (ret != 0)
    {
        return ret;
    }

    bool set_as_latest_entry_for_key;
    uint32_t replaced_block;
    ret = kv_directory__store_key(store->directory, k->id, destination_block, v->size, my_sequence, &set_as_latest_entry_for_key, &replaced_block);
    if (ret != 0)
    {
        return ret;
    }

    if (replaced_block != UINT32_MAX)
    {
        kv_block_allocator__free_block(store->block_allocator, replaced_block);
    }

    assert(set_as_latest_entry_for_key);
    kv_block_allocator__mark_block_as_allocated(store->block_allocator, destination_block);

    return 0;
}

int kv_del(struct kvstor *store, const struct key *k)
{
    uint32_t old_block;
    size_t size;
    int ret = kv_directory__lookup_key(store->directory, k->id, &old_block, &size);
    if (ret != 0)
    {
        return ret;
    }

    struct kv_block *block_data = (struct kv_block *)malloc(sizeof(struct kv_block));
    kv_block__init(block_data, k->id, 0, NULL, store->current_sequence_number++);
    /// @todo DeleteOnExit<kv_block> on_exit(block_data);

    uint32_t destination_block = kv_append_point__get_append_point(store->append_point);
    if (destination_block == UINT32_MAX)
    {
        return -ENOSPC;
    }

    ret = kv_block_array__write_block(store->block_array, destination_block, (const uint8_t *)block_data);
    if (ret != 0)
    {
        return ret;
    }

    ret = kv_directory__remove_key(store->directory, k->id);
    if (ret != 0)
    {
        return ret;
    }

    kv_block_allocator__free_block(store->block_allocator, old_block);

    return 0;
}
