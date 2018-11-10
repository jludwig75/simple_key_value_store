#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

enum {
	MAXKEYS = 100000,
	MAXBLOB = 4 * 1024,
};

struct kvstor;

struct key {
	uint64_t id;
};

struct value {
	size_t  size;
	char    data[];
};

/* forward declare the access functions */
extern int    kv_open(struct kvstor **storpp, bool create,
	int argc, char **argv);
extern void    kv_close(struct kvstor *stor);
extern int    kv_get(struct kvstor *stor,
	const struct key *k, struct value *v);
extern int    kv_set(struct kvstor *stor,
	const struct key *k, const struct value *v);
extern int    kv_del(struct kvstor *stor, const struct key *k);
