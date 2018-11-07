#include "CppUTest/TestHarness.h"
#include "CppUTest/CommandLineTestRunner.h"

#include "BlockAllocator.h"

TEST_GROUP(BlockAllocator)
{
};

TEST(BlockAllocator, all_blocks_start_free)
{
	BlockAllocator allocator(15);

	CHECK_EQUAL(0, allocator.find_next_free_block(0));
	CHECK_EQUAL(1, allocator.find_next_free_block(1));
	CHECK_EQUAL(13, allocator.find_next_free_block(13));
	CHECK_EQUAL(14, allocator.find_next_free_block(14));
}

TEST(BlockAllocator, find_next_free_block__skips_allocated_block)
{
	BlockAllocator allocator(15);

	allocator.mark_block_as_allocated(0);
	CHECK_EQUAL(1, allocator.find_next_free_block(0));
}

TEST(BlockAllocator, find_next_free_block__skips_allocated_block_at_end_and_wraps_to_beginning_block)
{
	BlockAllocator allocator(15);

	allocator.mark_block_as_allocated(14);
	CHECK_EQUAL(0, allocator.find_next_free_block(14));
}

TEST(BlockAllocator, find_next_free_block__skips_range_of_allocated_blocks_that_wrap_to_block_0)
{
	BlockAllocator allocator(15);

	allocator.mark_block_as_allocated(11);
	allocator.mark_block_as_allocated(12);
	allocator.mark_block_as_allocated(13);
	allocator.mark_block_as_allocated(14);
	CHECK_EQUAL(0, allocator.find_next_free_block(11));
}

TEST(BlockAllocator, find_next_free_block__skips_range_of_allocated_blocks_that_wrap_to_block_2)
{
	BlockAllocator allocator(15);

	allocator.mark_block_as_allocated(0);
	allocator.mark_block_as_allocated(1);
	allocator.mark_block_as_allocated(13);
	allocator.mark_block_as_allocated(14);
	CHECK_EQUAL(2, allocator.find_next_free_block(13));
}

TEST(BlockAllocator, find_next_free_block__returns_UINT32_MAX_when_all_blocks_are_allocated)
{
	BlockAllocator allocator(5);

	allocator.mark_block_as_allocated(0);
	allocator.mark_block_as_allocated(1);
	allocator.mark_block_as_allocated(2);
	allocator.mark_block_as_allocated(3);
	allocator.mark_block_as_allocated(4);
	CHECK_EQUAL(UINT32_MAX, allocator.find_next_free_block(0));
	CHECK_EQUAL(UINT32_MAX, allocator.find_next_free_block(3));
}

TEST(BlockAllocator, find_next_free_block__block_can_be_allocated_after_it_is_freed)
{
	BlockAllocator allocator(5);

	allocator.mark_block_as_allocated(0);
	allocator.mark_block_as_allocated(1);
	allocator.mark_block_as_allocated(2);
	allocator.mark_block_as_allocated(3);
	allocator.mark_block_as_allocated(4);
	CHECK_EQUAL(UINT32_MAX, allocator.find_next_free_block(3));
	allocator.free_block(2);
	CHECK_EQUAL(2, allocator.find_next_free_block(3));
}

TEST(BlockAllocator, find_next_free_block__block_0_can_be_allocated_after_it_is_freed)
{
	BlockAllocator allocator(5);

	allocator.mark_block_as_allocated(0);
	allocator.mark_block_as_allocated(1);
	allocator.mark_block_as_allocated(2);
	allocator.mark_block_as_allocated(3);
	allocator.mark_block_as_allocated(4);
	CHECK_EQUAL(UINT32_MAX, allocator.find_next_free_block(3));
	allocator.free_block(0);
	CHECK_EQUAL(0, allocator.find_next_free_block(3));
}

TEST(BlockAllocator, find_next_free_block__last_block_can_be_allocated_after_it_is_freed)
{
	BlockAllocator allocator(5);

	allocator.mark_block_as_allocated(0);
	allocator.mark_block_as_allocated(1);
	allocator.mark_block_as_allocated(2);
	allocator.mark_block_as_allocated(3);
	allocator.mark_block_as_allocated(4);
	CHECK_EQUAL(UINT32_MAX, allocator.find_next_free_block(3));
	allocator.free_block(4);
	CHECK_EQUAL(4, allocator.find_next_free_block(3));
}


int main(int argc, char *argv[])
{
	return CommandLineTestRunner::RunAllTests(argc, argv);
}
