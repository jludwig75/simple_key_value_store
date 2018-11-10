#include "tsearch.h"

#include <set>

struct t_node
{
	t_node(void *data) : data(data)
	{
	}
	void *data;
};

struct node_container
{
	t_node *node;
	struct t_root *root;
};

struct t_root
{
	t_root(compare_node_call compare_node) : nodes(), compare_node(compare_node)
	{
	}
	std::set<node_container> nodes;
	compare_node_call compare_node;
};

bool operator <(const struct node_container &a, const struct node_container &b)
{
	return a.root->compare_node(a.node->data, b.node->data) == -1;
}


void *tsearch(void *new_entry, void **_entries_root, compare_node_call compare_node)
{
	if (!_entries_root)
	{
		return NULL;
	}

	if (!*_entries_root)
	{
		*_entries_root = new t_root(compare_node);
		if (!*_entries_root)
		{
			return NULL;
		}
	}

	t_root *base = (t_root *)*_entries_root;

	node_container container;
	container.node = new t_node(new_entry);
	container.root = base;

	std::set<node_container>::iterator i = base->nodes.find(container);
	if (i != base->nodes.end())
	{
		delete container.node;
		return i->node;
	}

	base->nodes.insert(container);
	return container.node;
}

void *tfind(const void *new_entry, void * const *_entries_root, compare_node_call compare_node)
{
	if (!_entries_root || !*_entries_root)
	{
		return NULL;
	}

	t_root *base = (t_root *)*_entries_root;

	t_node dummy((void *)new_entry);
	node_container search_key;
	search_key.node = &dummy;
	search_key.root = base;

	std::set<node_container>::iterator i = base->nodes.find(search_key);
	if (i == base->nodes.end())
	{
		return NULL;
	}

	return i->node;
}

void *tdelete(const void *entry, void * *_entries_root, compare_node_call compare_node)
{
	if (!_entries_root || !*_entries_root)
	{
		return NULL;
	}

	t_root *base = (t_root *)*_entries_root;

	t_node dummy((void *)entry);
	node_container search_key;
	search_key.node = &dummy;
	search_key.root = base;

	std::set<node_container>::iterator i = base->nodes.find(search_key);
	if (i == base->nodes.end())
	{
		return NULL;
	}

	t_node *node = i->node;
	base->nodes.erase(i);
	delete node;
	return (void*)node;	// DON'T DEREFRENCE
}

void tdestroy(void *_entries_root, free_node_call free_node)
{
	if (!_entries_root)
	{
		return;
	}

	t_root *base = (t_root *)_entries_root;
	for (std::set<node_container>::iterator i = base->nodes.begin(); i != base->nodes.end(); )
	{
		t_node *node = i->node;
		i = base->nodes.erase(i);
		free_node(node->data);
		delete node;
	}

	delete base;
}

