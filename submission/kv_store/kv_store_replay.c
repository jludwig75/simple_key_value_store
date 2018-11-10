#include "kv_store_replay.h"

#include "kv_block.h"


int kv_store__replay_log(struct kvstor *store)
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
