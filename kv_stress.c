#include "kv_store.h"

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>


enum operation { op_set, op_get, op_del, op_max };


enum operation select_operation()
{
	switch(rand() % 6)
	{
	case 0:	// Make this workload set heavy so we're sure to fill up and get failures.
	case 1:
	case 2:
		return op_set;
	case 3:
	case 4:
		return op_get;
	case 5:
		return op_del;
	default:
		assert(false);
		return op_max;
	}
}

size_t select_data_size()
{
	return (rand() % MAXBLOB) + 1;
}

void fill_random_data(struct value *v)
{
	size_t i;

	for(i = 0; i < v->size; i++)
	{
		v->data[i] = (char)(rand() % UINT8_MAX);
	}
}

void generate_random_value(struct value *v)
{
	v->size = select_data_size();
	fill_random_data(v);
}

bool compare_values(struct value *a, struct value *b)
{
	// First check the size. We can't memcmp
	// the whole structure until we know they are
	// the same size. We might overrun one of the buffers.
	if (a->size != b->size)
	{
		return false;
	}

	return memcmp(a->data, b->data, a->size) == 0;
}

// We want a small range to force overlaps
const size_t max_unique_keys = 1000;
uint64_t select_key()
{
	return rand() % max_unique_keys;
}

struct test_value
{
	union {
		struct
		{
			size_t size;
			char data[MAXBLOB];
		};
		struct value value;
	};
};

int do_set(struct kvstor *stor)
{
	struct key key;
	key.id = select_key();
	struct test_value value;

	generate_random_value(&value.value);

	int ret = kv_set(stor, &key, &value.value);
	if (ret != 0 && ret != -ENOSPC)
	{
		return ret;
	}

	return 0;
}

int do_get(struct kvstor *stor)
{
	struct key key;
	key.id = select_key();
	struct test_value value;
	value.value.size = sizeof(value.data);

	int ret = kv_get(stor, &key, &value.value);
	if (ret != 0 && ret != -ENOENT)
	{
		return ret;
	}

	return 0;
}

int do_del(struct kvstor *stor)
{
	struct key key;
	key.id = select_key();

	int ret = kv_del(stor, &key);
	if (ret != 0 && ret != -ENOENT)
	{
		return ret;
	}

	return 0;
}

int run_stress(size_t iterations, int argc, char **argv, size_t *tests_run, size_t *failures)
{
	struct kvstor *stor;
	*failures = 0;
	*tests_run = 0;

	int ret = kv_open(&stor, true, argc, argv);
	if (ret != 0)
	{
		return 1;
	}

	size_t i;
	for(i = 0; i < iterations; i++)
	{
		enum operation op = select_operation();
		switch(op)
		{
		case op_set:
			ret = do_set(stor);
			if (ret != 0)
			{
				printf("do_set failed: %d\n", ret);
				*failures = * failures + 1;
			}
			*tests_run = *tests_run + 1;
			break;
		case op_get:
			ret = do_set(stor);
			if (ret != 0)
			{
				printf("do_get failed: %d\n", ret);
				*failures = * failures + 1;
			}
			*tests_run = *tests_run + 1;
			break;
		case op_del:
			ret = do_del(stor);
			if (ret != 0)
			{
				printf("do_del failed: %d\n", ret);
				*failures = * failures + 1;
			}
			*tests_run = *tests_run + 1;
			break;
		case op_max:
			break;
		}
	}

	kv_close(stor);

	return 0;
}


int main(int argc, char **argv)
{
	unsigned seed = (unsigned)time(NULL);
	printf("Running with seed %u\n", seed);
	srand(seed);

	size_t tests_run, failures;
	int ret = run_stress(1000000, argc, argv, &tests_run, &failures);
	if (ret != 0)
	{
		fprintf(stderr, "Failed to run test suite: %d.\n", ret);
		return ret;
	}

	printf("Tan %lu tests with %lu failures\n", tests_run, failures);
	return 0;
}
