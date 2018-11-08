#include "kv_store.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

/* simple standalone test framework */
static unsigned int testnum;
static unsigned int testfail;

static int
__vtest(const char *file, int line, int test, const char *fmt, va_list args)
{
	int sz;
	va_list atmp;

	testnum += 1;
	printf("%sok %d", !test ? "not " : "", testnum);

	va_copy(atmp, args);
	sz = vsnprintf(NULL, 0, fmt ? fmt : "", atmp);
	if (sz != 0) {
		printf(" - ");
		printf(fmt ? fmt : "", atmp);
	}
	va_end(atmp);
	printf("\n");

	if (!test) {
		printf("#   Failed test ");
		va_copy(atmp, args);
		sz = vsnprintf(NULL, 0, fmt ? fmt : "", atmp);
		if (sz != 0) {
			printf("'");
			printf(fmt ? fmt : "", atmp);
			printf("'\n#   ");
		}
		va_end(atmp);
		printf("at %s line %d.\n", file, line);

		testfail += 1;
	}

	return test;
}

static int
__test(const char *file, int line, int test, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	__vtest(file, line, test, fmt, args);
	va_end(args);
	return test;
}

#define test(...)   __test(__FILE__, __LINE__, __VA_ARGS__, NULL)

struct value_buffer
{
	size_t  size;
	char    data[MAXBLOB];
};

int kv_set_get_cmp(struct kvstor *stor)
{
	struct key k = { 54689 } ;

	struct value_buffer input_buffer;
	struct value *input_value = (struct value *)&input_buffer;
	strcpy(input_value->data, "This is a test value.");
	input_value->size = strlen(input_value->data);

	int ret = kv_set(stor, &k, input_value);
	if (ret != 0)
	{
		return ret;
	}

	struct value_buffer output_value_buffer;
	struct value *output_value = (struct value *)&output_value_buffer;
	ret = kv_get(stor, &k, output_value);
	if (ret != 0)
	{
		return ret;
	}

	if (input_value->size != output_value->size || memcmp(input_value->data, output_value->data, input_value->size) != 0)
	{
		return -1;
	}

	return kv_del(stor, &k);
}

int kv_set__can_set_max_values(struct kvstor *stor)
{
	uint64_t i;
	for(i = 0; i < MAXKEYS; i++)
	{
		struct key k;
		k.id = i;

		struct value_buffer value_buffer;
		struct value *value = (struct value *)&value_buffer;
		memset(value->data, i, MAXBLOB);
		value->size = MAXBLOB;

		int ret = kv_set(stor, &k, value);
		if (ret != 0)
		{
			return ret;
		}
	}

	for(i = 0; i < MAXKEYS; i++)
	{
		struct key k;
		k.id = i;

		int ret = kv_del(stor, &k);
		if (ret != 0)
		{
			return ret;
		}
	}
	return 0;
}

int kv_set__can_replace_max_values(struct kvstor *stor)
{
	uint64_t i;
	size_t r;
	const size_t test_repetitions = 5;
	for(r = 0; r < test_repetitions; r++)
	{
		int jrl = 43;
		(void)jrl;
		for(i = 0; i < MAXKEYS; i++)
		{
			struct key k;
			k.id = i;

			struct value_buffer value_buffer;
			struct value *value = (struct value *)&value_buffer;
			memset(value->data, i, MAXBLOB);
			value->size = MAXBLOB;

			int ret = kv_set(stor, &k, value);
			if (ret != 0)
			{
				printf("Failed to set key %u on repetition %lu\n", (unsigned)i, r);
				return ret;
			}
		}
	}

	for(i = 0; i < MAXKEYS; i++)
	{
		struct key k;
		k.id = i;

		int ret = kv_del(stor, &k);
		if (ret != 0)
		{
			return ret;
		}
	}
	return 0;
}

int
main(int argc, char **argv)
{
	struct kvstor *stor;

	/* run the test vectors */
	test(kv_open(&stor, true, argc, argv) == 0, "kv_open");

	struct key k = { 123 } ;

	struct value_buffer input_buffer;
	struct value *input_value = (struct value *)&input_buffer;
	strcpy(input_value->data, "This is a test value.");
	input_value->size = strlen(input_value->data);

	test(kv_set(stor, &k, input_value) == 0, "kv_set");

	struct value_buffer output_value_buffer;
	struct value *output_value = (struct value *)&output_value_buffer;
	test(kv_get(stor, &k, output_value) == 0, "kv_get");

	test(kv_del(stor, &k) == 0, "kv_del");

	test(kv_set_get_cmp(stor) == 0, "kv_set_get_cmp");

	test(kv_set__can_set_max_values(stor) == 0, "kv_set__can_set_max_values");

	test(kv_set__can_replace_max_values(stor) == 0, "kv_set__can_replace_max_values");

	kv_close(stor);

	printf("%u failed out of %u run\n", testfail, testnum);
	return 0;
}
