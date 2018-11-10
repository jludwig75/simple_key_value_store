#include "CppUTest/TestHarness.h"
#include "CppUTest/CommandLineTestRunner.h"

#include "tsearch.h"

TEST_GROUP(tsearch)
{
};

static int compare_nodes(const void *nodea, const void *nodeb)
{
	if (*(const int*)nodea < *(const int*)nodeb)
	{
		return -1;
	}
	if (*(const int*)nodea > *(const int*)nodeb)
	{
		return 0;
	}
	return 0;
}

void free_node(void *node)
{
	delete (int*)node;
}

TEST(tsearch, adds_new_node)
{
	void *root = NULL;
	int *data = new int(99);

	void *new_node = tsearch(data, &root, compare_nodes);
	CHECK(new_node != NULL);
	CHECK(root != NULL);

	tdestroy(root, free_node);
}

TEST(tsearch, fails_to_find_non_existant_value)
{
	void *root = NULL;
	int *data = new int(99);

	void *new_node = tsearch(data, &root, compare_nodes);
	CHECK(new_node != NULL);
	CHECK(root != NULL);

	int key = 12;
	CHECK(NULL == tfind(&key, &root, compare_nodes));

	tdestroy(root, free_node);
}

TEST(tsearch, fails_to_delete_non_existant_value)
{
	void *root = NULL;
	int *data = new int(99);

	void *new_node = tsearch(data, &root, compare_nodes);
	CHECK(new_node != NULL);
	CHECK(root != NULL);

	int key = 12;
	CHECK(NULL == tdelete(&key, &root, compare_nodes));

	tdestroy(root, free_node);
}

TEST(tsearch, can_add_and_retrieve_new_node)
{
	void *root = NULL;
	int *data = new int(99);

	void *new_node = tsearch(data, &root, compare_nodes);
	CHECK(new_node != NULL);
	CHECK(root != NULL);

	int key = 99;
	int **node = (int **)tfind(&key, &root, compare_nodes);
	CHECK(node != NULL);
	CHECK(*node != NULL);
	CHECK_EQUAL(99, **node);

	tdestroy(root, free_node);
}

TEST(tsearch, can_add_and_delete_new_node)
{
	void *root = NULL;
	int *data = new int(99);

	void *new_node = tsearch(data, &root, compare_nodes);
	CHECK(new_node != NULL);
	CHECK(root != NULL);

	int key = 99;
	int **node = (int **)tfind(&key, &root, compare_nodes);
	CHECK(node != NULL);
	CHECK(*node != NULL);
	CHECK_EQUAL(99, **node);

	void *ret = tdelete(&key, &root, compare_nodes);
	CHECK(ret != NULL);
	delete data;

	ret = tfind(&key, &root, compare_nodes);
	CHECK(NULL == ret);

	tdestroy(root, free_node);
}

#define NUM_ELEMENTS(arr)	(sizeof(arr) / sizeof(arr[0]))

void do_not_free_node(void *node)
{
}

TEST(tsearch, can_add_and_retrieve_multiple_values)
{
	void *root = NULL;
	int test_values[] = { 0, 4, 17, 3, 2, 19, 99 };

	for (size_t i = 0; i < NUM_ELEMENTS(test_values); i++)
	{
		int **node = (int **)tsearch(&test_values[i], &root, compare_nodes);
		CHECK(node != NULL);
		CHECK_EQUAL(test_values[i] , **node);
	}

	for (size_t i = 0; i < NUM_ELEMENTS(test_values); i++)
	{
		int **node = (int **)tfind(&test_values[i], &root, compare_nodes);
		CHECK(node != NULL);
		CHECK_EQUAL(test_values[i], **node);

		CHECK(NULL != tdelete(&test_values[i], &root, compare_nodes));
	}

	for (size_t i = 0; i < NUM_ELEMENTS(test_values); i++)
	{
		CHECK(NULL == tfind(&test_values[i], &root, compare_nodes));
	}

	tdestroy(root, do_not_free_node);
}

TEST(tsearch, reuses_new_entry_when_item_with_duplicate_value_added)
{
	void *root = NULL;
	int item1 = 77;
	int item2 = item1;

	int **found_item_node1 = (int **)tsearch(&item1, &root, compare_nodes);
	CHECK(NULL != found_item_node1);
	CHECK(NULL != *found_item_node1);
	CHECK_EQUAL(item1, **found_item_node1);

	int **found_item_node2 = (int **)tsearch(&item2, &root, compare_nodes);
	CHECK(NULL != found_item_node2);
	CHECK(NULL != *found_item_node2);
	// Make sure the previous node was reused.
	CHECK_EQUAL(found_item_node1, found_item_node2);
	CHECK_EQUAL(item1, **found_item_node2);

	tdestroy(root, do_not_free_node);
}

int main(int argc, char *argv[])
{
	//MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
	return CommandLineTestRunner::RunAllTests(argc, argv);
}
