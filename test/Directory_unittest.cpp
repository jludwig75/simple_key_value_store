#include "CppUTest/TestHarness.h"
#include "CppUTest/CommandLineTestRunner.h"

#include "Directory.h"

#include <errno.h>


TEST_GROUP(Directory)
{
};

TEST(Directory, store_key__works_until_full)
{
	Directory dir(5);

	CHECK_EQUAL(0, dir.store_key( 0, 19, 334));
	CHECK_EQUAL(0, dir.store_key(10, 19, 334));
	CHECK_EQUAL(0, dir.store_key(20, 19, 334));
	CHECK_EQUAL(0, dir.store_key(30, 19, 334));
	CHECK_EQUAL(0, dir.store_key(40, 19, 334));
	CHECK_EQUAL(-ENOSPC, dir.store_key(50, 19, 334));
}

TEST(Directory, stored_keys_can_be_retrieved_with_correct_values)
{
	const size_t test_max_dir_entries = 5;

	uint32_t dir_entry_blocks[] = {19, 111, 219, 319, 419};
	size_t dir_entry_byte_counts[] = {100, 111, 222, 333, 444};
	Directory dir(test_max_dir_entries);

	for(size_t i = 0; i < test_max_dir_entries; i++)
	{
		uint64_t key = i * 10;
		CHECK_EQUAL(0, dir.store_key(key, dir_entry_blocks[i], dir_entry_byte_counts[i]));
	}

	for(uint64_t i = 0; i < test_max_dir_entries; i++)
	{
		uint32_t block;
		size_t bytes;
		uint64_t key = i * 10;
		CHECK_EQUAL(0, dir.lookup_key(key, &block, &bytes));
		CHECK_EQUAL(dir_entry_blocks[i], block);
		CHECK_EQUAL(dir_entry_byte_counts[i], bytes);
	}
}

TEST(Directory, returns_ENOENT_for_key_not_stored_when_directory_is_empty)
{
	Directory dir(5);

	uint32_t block;
	size_t bytes;
	CHECK_EQUAL(-ENOENT, dir.lookup_key(99, &block, &bytes));
}

TEST(Directory, returns_ENOENT_for_key_not_stored_when_directory_is_full)
{
	Directory dir(5);

	CHECK_EQUAL(0, dir.store_key( 0, 19, 334));
	CHECK_EQUAL(0, dir.store_key(10, 19, 334));
	CHECK_EQUAL(0, dir.store_key(20, 19, 334));
	CHECK_EQUAL(0, dir.store_key(30, 19, 334));
	CHECK_EQUAL(0, dir.store_key(40, 19, 334));

	uint32_t block;
	size_t bytes;
	CHECK_EQUAL(-ENOENT, dir.lookup_key(99, &block, &bytes));
}

TEST(Directory, cannot_find_key_after_it_is_removed)
{
	Directory dir(5);

	const uint64_t test_key = 20345;
	const uint32_t test_block = 15439;
	const size_t test_bytes = 43;
	// Store the key
	CHECK_EQUAL(0, dir.store_key(test_key, test_block, test_bytes));

	uint32_t block;
	size_t bytes;
	// We can look it up
	CHECK_EQUAL(0, dir.lookup_key(test_key, &block, &bytes));
	CHECK_EQUAL(test_block, block);
	CHECK_EQUAL(test_bytes, bytes);

	// Remove the key from the dictionary
	CHECK_EQUAL(0, dir.remove_key(test_key));

	// Now we can't look it up.
	CHECK_EQUAL(-ENOENT, dir.lookup_key(test_key, &block, &bytes));
}

TEST(Directory, remove_key__returns_ENOENT_if_key_not_stored)
{
	Directory dir(5);

	uint32_t block;
	size_t bytes;
	CHECK_EQUAL(-ENOENT, dir.lookup_key(546, &block, &bytes));
}

int main(int argc, char *argv[])
{
	return CommandLineTestRunner::RunAllTests(argc, argv);
}
