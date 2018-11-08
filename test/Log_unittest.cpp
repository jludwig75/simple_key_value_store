#include "CppUTest/TestHarness.h"
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTestExt/MockSupport.h"

#include "Log.h"
#include "BlockArray.h"

#include <errno.h>
#include <string.h>


TEST_GROUP(Log)
{
    void setup()
    {
    }
	void teardown()
    {
        mock().checkExpectations();
        mock().clear();
    }
};

TEST(Log, format_truncates_block_array_file)
{
	BlockArray block_array(Log::get_raw_block_size());
	Log log(&block_array);

	mock().expectOneCall("BlockArray::truncate").andReturnValue(22);

	CHECK_EQUAL(22, log.format());

}

TEST(Log, read_block__reads_block_from_block_array_and_validates_it)
{
	BlockArray block_array(Log::get_raw_block_size());
	Log log(&block_array);

	const uint32_t test_block = 44;
	mock().expectOneCall("BlockArray::read_block").
			withParameter("block", test_block).
			ignoreOtherParameters().
			andReturnValue(0);


	mock().expectOneCall("kv_block::validate").
			andReturnValue(true);

	char test_buffer[56];
	CHECK_EQUAL(0, log.read_block(test_block, test_buffer, sizeof(test_buffer)))
}

TEST(Log, read_block__returns_error_and_exits_if_block_read_from_block_array_fails)
{
	BlockArray block_array(Log::get_raw_block_size());
	Log log(&block_array);

	const uint32_t test_block = 44;
	mock().expectOneCall("BlockArray::read_block").
			withParameter("block", test_block).
			ignoreOtherParameters().
			andReturnValue(-EIO);


	char test_buffer[56];
	CHECK_EQUAL(-EIO, log.read_block(test_block, test_buffer, sizeof(test_buffer)))
}

TEST(Log, read_block__returns_EFAULT_if_block_validation_fails)
{
	BlockArray block_array(Log::get_raw_block_size());
	Log log(&block_array);

	const uint32_t test_block = 44;
	mock().expectOneCall("BlockArray::read_block").
			withParameter("block", test_block).
			ignoreOtherParameters().
			andReturnValue(0);


	mock().expectOneCall("kv_block::validate").
			andReturnValue(false);

	char test_buffer[56];
	CHECK_EQUAL(-EFAULT, log.read_block(test_block, test_buffer, sizeof(test_buffer)))
}

TEST(Log, write_key_value__finds_next_free_block_and_writes_to_that_block)
{
	BlockArray block_array(Log::get_raw_block_size());
	Log log(&block_array);


	uint32_t new_block = 234;
	mock().expectOneCall("BlockAllocator::find_next_free_block").
			ignoreOtherParameters().
			andReturnValue(new_block);

	mock().expectOneCall("BlockArray::write_block").
			withParameter("destination_block", new_block).
			ignoreOtherParameters().
			andReturnValue(0);

	uint64_t key = 0x5423785934;
	const char *test_value = "This is a test value";
	uint32_t block_written;
	CHECK_EQUAL(0, log.write_key_value(key, test_value, strlen(test_value), &block_written))
}

int main(int argc, char *argv[])
{
	return CommandLineTestRunner::RunAllTests(argc, argv);
}
