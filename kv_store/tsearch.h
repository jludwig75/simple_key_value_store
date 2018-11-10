#pragma once

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


typedef void(*free_node_call)(void *node);
int typedef(*compare_node_call)(const void *pa, const void *pb);

void *tsearch(void *entry, void **root, compare_node_call compare_node);

void *tfind(const void *entry, void * const *root, compare_node_call compare_node);

void *tdelete(const void *entry, void **root, compare_node_call compare_node);

void tdestroy(void *root, free_node_call free_node);

#ifdef __cplusplus
}    // extern "C"
#endif // __cplusplus
