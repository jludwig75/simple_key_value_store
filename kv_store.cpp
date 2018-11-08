#include "kv_store.h"

#include "KvStore.h"


struct kvstor
{
	kvstor(const char *file_name, bool create) :
		store(file_name, create)
	{
	}
	KvStore store;
};

int	kv_open(struct kvstor **storpp, bool create,
			int argc, char **argv)
{
	kvstor *stor = new kvstor(argv[1], create);

	int ret;
	if (create)
	{
		ret = stor->store.format();
	}
	else
	{
		ret = stor->store.load();
	}

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

void kv_close(struct kvstor *stor)
{
	stor->store.close();
	delete stor;
}

int	kv_get(struct kvstor *stor,
		       const struct key *k, struct value *v)
{
	return stor->store.get(k->id, v->data, &v->size);
}

int	kv_set(struct kvstor *stor,
		       const struct key *k, const struct value *v)
{
	return stor->store.set(k->id, v->data, v->size);
}

int	kv_del(struct kvstor *stor, const struct key *k)
{
	return stor->store.del(k->id);
}

