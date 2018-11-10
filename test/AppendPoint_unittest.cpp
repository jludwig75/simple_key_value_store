#include "CppUTest/TestHarness.h"
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTestExt/MockSupport.h"

#include "AppendPoint.h"
#include "BlockAllocator.h"

TEST_GROUP(AppendPoint)
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

TEST(AppendPoint, get_append_point__checks_append_point_with_allocator)
{
	BlockAllocator allocator(1000);
	AppendPoint appendPoint(&allocator);

	mock().expectOneCall("BlockAllocator::find_next_free_block").
		withParameter("starting_block", 0).
		andReturnValue(97);

	CHECK_EQUAL(97, appendPoint.get_append_point());
}

TEST(AppendPoint, update_append_point__retains_highest_sequence_append_point)
{
	BlockAllocator allocator(1000);
	AppendPoint appendPoint(&allocator);

	appendPoint.update_append_point(5, 6);
	appendPoint.update_append_point(15, 8);
	appendPoint.update_append_point(4, 4);
	appendPoint.update_append_point(555, 3);

	mock().expectOneCall("BlockAllocator::find_next_free_block").
		withParameter("starting_block", 15).
		andReturnValue(15);

	CHECK_EQUAL(15, appendPoint.get_append_point());
}

int main(int argc, char *argv[])
{
	return CommandLineTestRunner::RunAllTests(argc, argv);
}
