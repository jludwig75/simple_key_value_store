#include "kv_store.h"

#include "KvStore.h"


struct kvstor
{
	KvStore store;
};

int	kv_open(struct kvstor **storpp, bool create,
			int argc, char **argv)
{
	kvstor *stor = new kvstor;

	int ret = stor->store.open(argv[1], create);
	if (ret == 0)
	{
		*storpp = stor;
	}
	else
	{
		delete stor;
	}
	return ret;
}

extern "C"
void kv_close(struct kvstor *stor)
{
	stor->store.close();
	delete stor;
}

extern "C"
int	kv_get(struct kvstor *stor,
		       const struct key *k, struct value *v)
{
	return stor->store.get(k->id, v->data, v->size, &v->size);
}

extern "C"
int	kv_set(struct kvstor *stor,
		       const struct key *k, const struct value *v)
{
	return stor->store.set(k->id, v->data, v->size);
}

extern "C"
int	kv_del(struct kvstor *stor, const struct key *k)
{
	return stor->store.del(k->id);
}

