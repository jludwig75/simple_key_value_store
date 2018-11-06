#include "kv_store.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>

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

int
main(int argc, char **argv)
{
	struct kvstor *stor;

	/* run the test vectors */
	test(kv_open(&stor, true, argc, argv) == 0, "kv_open");

	kv_close(stor);

	printf("%u failed out of %u run\n", testfail, testnum);
	return 0;
}
