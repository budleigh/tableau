#include <stdio.h>
#include "table.h"

int main() {
	Table t = create_table();
	int key = 123;
	int val = 123;
	char hint[] = "yes";
	insert(&t, &key, &val, hint, sizeof(key), sizeof(val));
	KVPair check = retrieve(&t, &key, sizeof(key));
	printf("%d\n", check.key);
	printf("%d\n", *(int *)check.val);
	printf("%s\n", check.val_type_hint);
	del(&t, &key, sizeof(key));
	KVPair check2 = retrieve(&t, &key, sizeof(key));
	printf("%d", check2.found_flag);
	return 0;
}
