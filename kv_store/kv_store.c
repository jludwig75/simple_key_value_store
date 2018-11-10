#include "kv_store.h"

#include "kv_store_class.h"


struct kvstor
{
	struct kv_store *store;
};

int	kv_open(struct kvstor **storpp, bool create,
			int argc, char **argv)
{
	struct kvstor *stor = (struct kvstor *)malloc(sizeof(struct kvstor));

	int ret = kv_store__init(&stor->store);
	if (ret != 0)
	{
		free(stor);
		return ret;
	}

	ret = kv_store__open(stor->store, argv[1], create);
	if (ret == 0)
	{
		*storpp = stor;
	}
	else
	{
		kv_store__cleanup(&stor->store);
		free(stor);
	}
	return ret;
}

void kv_close(struct kvstor *stor)
{
	kv_store__close(stor->store);
	kv_store__cleanup(&stor->store);
	free(stor);
}

int	kv_get(struct kvstor *stor,
		       const struct key *k, struct value *v)
{
	return kv_store__get(stor->store, k->id, v->data, v->size, &v->size);
}

int	kv_set(struct kvstor *stor,
		       const struct key *k, const struct value *v)
{
	return kv_store__set(stor->store, k->id, v->data, v->size);
}

int	kv_del(struct kvstor *stor, const struct key *k)
{
	return kv_store__del(stor->store, k->id);
}

