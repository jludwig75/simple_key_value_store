#include "kv_checker.h"

#define _GNU_SOURCE
#include <search.h>
#include <errno.h>
#include <string.h>
#include <assert.h>


struct kv_checker
{
	void *value_root;
};

struct kv_node
{
	struct key key;
	struct value *value;
};


static struct value *duplicate_value(const struct value *source)
{
	if (!source || source->size == 0)
	{
		return NULL;
	}

	size_t value_struct_size = sizeof(struct value) + source->size;
	struct value *new_value = malloc(value_struct_size);
	if (new_value)
	{
		memcpy(new_value, source, value_struct_size);
	}

	return new_value;
}

static int kv_node__compare(const void *pa, const void *pb)
{
	const struct kv_node *node_a = (const struct kv_node *)pa;
	const struct kv_node *node_b = (const struct kv_node *)pb;

	if (node_a->key.id < node_b->key.id)
	{
		return -1;
	}
	if (node_a->key.id > node_b->key.id)
	{
		return 1;
	}
	return 0;
}


int kv_checker__init(struct kv_checker **checker)
{
	*checker = malloc(sizeof(struct kv_checker));
	if (!*checker)
	{
		return -ENOMEM;
	}

	(*checker)->value_root = NULL;
	return 0;
}

void kv_checker__cleanup(struct kv_checker **checker)
{
//	if ((*checker)->value_root)
//	{
//		tdestroy((*checker)->value_root);
//	}

	free(*checker);
	*checker = NULL;
}

int kv_checker__store_value(struct kv_checker *checker, const struct key *k, const struct value *v)
{
	struct kv_node *search_node = malloc(sizeof(struct kv_node));
	if (!search_node)
	{
		return -ENOMEM;
	}
	search_node->key = *k;
	search_node->value = NULL;


	struct value *value_copy = duplicate_value(v);
	if (!value_copy)
	{
		return -ENOMEM;
	}

	struct kv_node **entry = tsearch(search_node, &checker->value_root, kv_node__compare);
	if (!entry)
	{
		// A new value could not be added.
		return -ENOMEM;
	}

	if (search_node != *entry)
	{
		free(search_node);
		assert((*entry)->value != NULL);
		free((*entry)->value);
		(*entry)->value = NULL;
	}

	assert((*entry)->value == NULL);
	(*entry)->value = value_copy;

	// Exit condition.
	assert((*entry)->value != NULL);

	return 0;
}

int kv_checker__check_value(const struct kv_checker *checker, const struct key *k, const struct value *v, bool *values_match)
{
	struct kv_node search_key;
	search_key.key = *k;

	struct kv_node **entry = tfind(&search_key, &checker->value_root, kv_node__compare);
	if (!entry)
	{
		*values_match = false;
		return -ENOENT;
	}

	const struct value *stored_value = (*entry)->value;
	assert(stored_value != NULL);

	*values_match = (stored_value->size == v->size && memcmp(stored_value->data, v->data, v->size) == 0);
	if (!*values_match)
	{
		int jrl = 43;
		(void)jrl;
	}
	return 0;
}

int kv_checker__delete_value(struct kv_checker *checker, const struct key *k)
{
	struct kv_node search_key;
	search_key.key = *k;

	struct kv_node **entry = tfind(&search_key, &checker->value_root, kv_node__compare);
	if (!entry)
	{
		return -ENOENT;
	}
	struct kv_node *node = *entry;

	if (tdelete(&search_key, &checker->value_root, kv_node__compare) == NULL)
	{
		return -ENOENT;
	}

	free(node);
	return 0;
}