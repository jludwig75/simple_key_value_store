#include "CppUTest/TestHarness.h"
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTestExt/MockSupport.h"

#include "KvStore.h"
#include "BlockArray.h"

#include <errno.h>
#include <string.h>


TEST_GROUP(KvStore_open)
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

TEST(KvStore_open, calls_block_array_open_with_file_name_and_create_flag)
{
	KvStore store;

	const char *test_file_name = "test_file_name";

	mock().expectOneCall("BlockArray::open").
		withStringParameter("file_name", test_file_name).
		withBoolParameter("create", true).
		andReturnValue(0);

	CHECK_EQUAL(0, store.open(test_file_name, true));
}

TEST(KvStore_open, returns_failue_if_block_array_open_fails)
{
	KvStore store;

	const char *test_file_name = "test_file_name";

	mock().expectOneCall("BlockArray::open").
		withStringParameter("file_name", test_file_name).
		withBoolParameter("create", false).
		andReturnValue(-77);

	CHECK_EQUAL(-77, store.open(test_file_name, false));
}

TEST(KvStore_open, scans_the_log_for_existing_store)
{
	KvStore store;

	const char *test_file_name = "test_file_name";

	mock().expectOneCall("BlockArray::open").
		withStringParameter("file_name", test_file_name).
		withBoolParameter("create", false).
		andReturnValue(0);

	// This proves that KvStore::scan was called,
	// but fail it so we don't have to mock it all.
	mock().expectOneCall("BlockArray::get_file_block_count").
		ignoreOtherParameters().
		andReturnValue(-65);

	CHECK_EQUAL(-65, store.open(test_file_name, false));
}


int main(int argc, char *argv[])
{
	return CommandLineTestRunner::RunAllTests(argc, argv);
}
