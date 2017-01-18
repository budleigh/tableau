#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "table.h"

/*
	TODO: - make sure insert is handling an
	overwrite situation - ie they insert a
	key that is already there so we overwrite
	the value.

	Also: make the strcpy calls safer!
*/

unsigned long hash(void *key, size_t key_size) {
	// djb2 algo, converted for use on any
	// arbitrary piece of data
	unsigned long hash = 5381;
	// hash it byte by byte regardless of type
	char *o_key = (char *)key;
	int c = *o_key;

	for (size_t x = 0; x < key_size; x++) {
		c = *o_key++;
		hash = ((hash << 5) + hash) + c;
	}

	return hash;
}

/*
Get the full hash of the given key, then modulo
it to the size of the given table to get a number
within the range 0-table_size.
*/
unsigned long get_hash_for_key(Table *t, void *key, size_t key_size) {
	return hash(key, key_size) % t->size;
}

Bucket *get_bucket_for_key(Table *t, void *key, size_t key_size) {
	unsigned int index = get_hash_for_key(t, key, key_size);
	Bucket *b = t->storage[index];
	return b;
}

/*
Sets up the bucket memory space that the table points to
as its storage. Theoretically, this function can be called
frequently as the table itself is resized - but when do we
trigger that resize? Having buckets makes 'full' a more
funky state to define.
*/
void initialize_buckets(Bucket *storage[], unsigned long size) {
	// this assumes storage has been allocated as well
	unsigned long x;

	for (x = 0; x < size; x++) {
		// can we get away with mallocing here?
		storage[x] = (Bucket *)calloc(1, sizeof(Bucket));

		// initialize the open store cache
		Bucket *b = storage[x];
		for (int y = 0; y < BUCK_SIZ; y++) {
			push(&b->open_indices, y);
		}
	}
}

void initialize_storage(Table *t) {
	// originally thought id do more here
	// oh well FUNCTIONAL PROGRAMMING
	initialize_buckets(t->storage, t->size);
}

Table create_table() {
	Table t;
	t.size = INIT_EXP;
	initialize_storage(&t);
	return t;
}

void stretch_table(Table *t) {
	t->size = t->size * 2;
	void *old_storage = t->storage;
	initialize_storage(t);
}

// this should be simplified greatly by structing the values + their sizes + hints
KVPair create_kvp(void *key, void *val, char hint[], size_t key_size, size_t val_size) {
	KVPair kvp;
	unsigned long real_hash = hash(key, key_size);
	kvp.key = real_hash;

	// since we dont know how this will get called
	// or the persistence of stack, we make our own
	// mem space for the value and use that instead
	kvp.val = malloc(val_size);
	memcpy(kvp.val, val, val_size);

	kvp.sizeof_key = key_size;
	kvp.sizeof_val = val_size;
	// todo: MSFT hates strcpy. figure this out
	strcpy(kvp.val_type_hint, hint);
	return kvp;
}

void empty_kvp(KVPair *kvp) {
	free(kvp->val);
	memset(kvp, 0, sizeof(KVPair));
}

void insert(Table *t, void *key, void *val, char hint[], size_t key_size, size_t val_size) {
	// create a permanent KVP memspace
	KVPair *p_kvp = malloc(sizeof(KVPair));
	*p_kvp = create_kvp(key, val, hint, key_size, val_size);

	// find our bucket and an open slot in it
	Bucket *b = get_bucket_for_key(t, key, key_size);
	// get a free index from the free index stack
	// those stacks are pre-populated with all of
	// the spaces to begin with
	int open_index = pop(&b->open_indices);
	if (open_index != -1) {
		b->storage[open_index] = p_kvp;
	}
	else {
		// TRIGGER RESIZE?! bucket-level or table?
	}
}

KVPair retrieve(Table *t, void *key, size_t key_size) {
	KVPair *result;
	// the 'real hash' is the full, unmodulo'd hash
	// it's used as the key instead of the actual key
	unsigned int real_hash = hash(key, key_size);
	Bucket *b = get_bucket_for_key(t, key, key_size);

	for (int x = 0; x < BUCK_SIZ; x++) {
		result = b->storage[x];
		if (result && (result->key == real_hash)) {
			result->found_flag = 1;
			return *result;
		}
	}

	// fell through, didnt find a matching key
	// hopefully the other side pays attention
	// to the found_flag value
	result->found_flag = 0;
	return *result;
}

void del(Table *t, void *key, size_t key_size) {
	unsigned long real_hash = hash(key, key_size);
	Bucket *b = get_bucket_for_key(t, key, key_size);

	for (int x = 0; x < BUCK_SIZ; x++) {
		KVPair *kvp = b->storage[x];
		if (kvp && (kvp->key == real_hash)) {
			empty_kvp(kvp);
			memset(&b->storage[x], 0, sizeof(KVPair *));
			// re-nominate this index as open
			push(&b->open_indices, x);
		}
	}
}
