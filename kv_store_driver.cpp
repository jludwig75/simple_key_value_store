#include "KvStore.h"

#include <stdio.h>
#include <string.h>


int main(int argc, char *argv[])
{
	KvStore store(argv[1], true);

	int ret = store.format();
	if (ret != 0)
	{
		printf("Error %d formatting key value store \"%s\"\n", ret, argv[1]);
		return ret;
	}

	const char *test_key = "Test key";
	store.set(0x298578934, test_key, strlen(test_key));

	return 0;
}

