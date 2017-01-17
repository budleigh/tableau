#ifndef TABLE
#define TABLE

#define INIT_EXP 64
#define BUCK_SIZ 16
#define HINT_SIZ 256

#include <stdlib.h>
#include "stack.h"

typedef struct _Table Table;
typedef struct _KVPair KVPair;
typedef struct _Bucket Bucket;

/********************
Generic storage for keys of any type and
values of any type. The keys are hashed
byte by byte, so they can be any type of
data - but if the other side mutates it,
it will generate a different hash (obv).

Values are void pointers and can be any
data type, I THINK including functions.
The only requirement is that you pass in
the sizeof the value you're pointing to
so that the table can allocate properly.
********************/
struct _KVPair {
	unsigned long key;  // hash
	void *val;
	unsigned long sizeof_key;
	unsigned long sizeof_val;
	char val_type_hint[HINT_SIZ];  // optional
	int found_flag;
};

/********************
Buckets are used to sort out hash
collisions. Each bucket is a fixed size
and when a bucket is full, we trigger a
table-wide storage resize.

They use 'free space caching' to
constant-tize bucket finding on insert.
Lookup is still technically linear in
the buckets themselves. I wonder if we can
use a hash table inside the buckets to
do better item lookups......
********************/
struct _Bucket {
	KVPair *storage[BUCK_SIZ];
	Stack open_indices;
};

struct _Table {
	Bucket *storage[INIT_EXP];
	unsigned long size;
};

unsigned long hash(void *key, size_t size);
Table  create_table();
void   insert(Table *t, void *key, void *val, char hint[], size_t key_size, size_t val_size);
KVPair retrieve(Table *t, void *key, size_t key_size);
void   del(Table *t, void *key, size_t key_size);

#endif
