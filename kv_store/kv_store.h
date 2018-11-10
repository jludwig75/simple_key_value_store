#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif    // __cplusplus

enum {
    MAXKEYS = 100000,
    MAXBLOB = 4 * 1024,
};

/* this should be defined in the implementation */
struct kvstor;

struct key {
    uint64_t id;
};

struct value {
    size_t  size;
    char    data[];
};

/* forward declare the access functions */
int    kv_open(struct kvstor **storpp, bool create,
            int argc, char **argv);
void    kv_close(struct kvstor *stor);
int    kv_get(struct kvstor *stor,
               const struct key *k, struct value *v);
int    kv_set(struct kvstor *stor,
               const struct key *k, const struct value *v);
int    kv_del(struct kvstor *stor, const struct key *k);

#ifdef __cplusplus
}    // extern "C"
#endif    // __cplusplus
