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

TEST(AppendPoint, update_append_point__retains_highest_sequence_append_point)
{
	BlockAllocator allocator(10);

	AppendPoint appendPoint(&allocator);

}

int main(int argc, char *argv[])
{
	return CommandLineTestRunner::RunAllTests(argc, argv);
}
