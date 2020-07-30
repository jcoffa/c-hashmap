#include "HashMap.h"

// A special hash entry value indicating no data is present in a bucket
static const HashEntry *const EMPTY_ENTRY = NULL;


// _DUMMY_ENTRY SHOULD NEVER BE USED DIRECTLY; it is only used to set the pointer constant DUMMY_ENTRY
static const HashEntry _DUMMY_ENTRY = {0, NULL, NULL};

/*
 * A special hash entry value indicating that data used to be present in thsi bucket,
 * but it has since been removed (by a call to `hashmapRemove` or `hashmapDeleteKey`).
 * This is necessary due to how linear probing breaks when a hole is introduced in
 * the middle of a sequence of filled buckets.
 */
static const HashEntry *const DUMMY_ENTRY = &_DUMMY_ENTRY;


static int64_t djb2x(void *key) {
	char *str = (char *)key;
	int64_t hash = 5381;
	int c;

	while ((c = *str++)) {
		hash = ((hash << 5) + hash) ^ c;	// (hash * 33) XOR c
	}

	return hash;
}

/*
 * Returns the smallest power of 2, N, such that: `x <= N`.
 *
 * For example, closestPow2(20) = 32 (32 is the smallest power of 2 that is >= 20)
 */
static int closestPow2(long x) {
	int toReturn = 1;

	while (toReturn < x) {
		toReturn *= 2;
	}

	return toReturn;
}


/*
 * Returns true if the given entry is either of the special hash entry values (EMPTY_ENTRY or DUMMY_ENTRY).
 *
 * Returns false otherwise, indicating that the entry contains legitimate data.
 */
static inline bool entryIsOpen(HashEntry *entry) {
	return (entry == EMPTY_ENTRY || entry == DUMMY_ENTRY);
}


HashMap *hashmapNew(int64_t (*hash)(void *), void (*deleteVal)(void *), char *(*printVal)(void *), \
        void (*deleteKey)(void *), char *(*printKey)(void *)) {

	return hashmapNewBuckets(DEFAULT_BUCKETS, hash, deleteVal, printVal, deleteKey, printKey);
}


HashMap *hashmapNewBuckets(long num_buckets, int64_t (*hash)(void *), void (*deleteValue)(void *), \
        char *(*printValue)(void *), void (*deleteKey)(void *), char *(*printKey)(void *)) {

	if (deleteValue == NULL || printValue == NULL || deleteKey == NULL || printKey == NULL) {
		return NULL;
	}

	HashMap *toReturn = malloc(sizeof(HashMap));

	if (toReturn == NULL) {
		return NULL;
	}

	toReturn->entries = malloc(sizeof(HashEntry) * closestPow2(num_buckets));
	if (toReturn->entries == NULL) {
		free(toReturn);
		return NULL;
	}

	// Fill the array with empty values
	for (long i = 0; i < num_buckets; i++) {
		(toReturn->entries)[i] = (HashEntry *)EMPTY_ENTRY;
	}

	// If hash function was not given, then use the djb2x hash function for string (char *) data
	if (hash == DEFAULT_HASH) {
		toReturn->hash = djb2x;
	} else {
		toReturn->hash = hash;
	}

	toReturn->length = 0;
	toReturn->num_buckets = num_buckets;
	toReturn->deleteValue = deleteValue;
	toReturn->printValue = printValue;
	toReturn->deleteKey = deleteKey;
	toReturn->printKey = printKey;

	return toReturn;
}


/*
 * Allocates and returns a new HashEntry to be put into a HashMap.
 *
 * Returns NULL instead if any memory allocation fails, or the key is NULL.
 * Hash map values are allowed to be NULL, but keys are not.
 */
static HashEntry *hashentryNew(int64_t hash, void *key, void *value) {
	if (key == NULL) {
		return NULL;
	}

	HashEntry *toReturn = malloc(sizeof(HashEntry));

	if (toReturn == NULL) {
		return NULL;
	}

	toReturn->hash = hash;
	toReturn->key = key;
	toReturn->value = value;

	return toReturn;
}


void hashmapClear(HashMap *map) {
	if (map == NULL) {
		return;
	}

	// This while loop's condition may seem odd, but this is an attempt to avoid useless iterations.
	// Since hash maps are full of empty space, it's not unreasonable for there to be a pretty
	// decent amount of empty buckets at the end of the hash map's entries array.
	//
	// This while loop condition basically says "while there are entries we haven't freed yet".
	HashEntry *entry;
	long i = 0;
	while (map->length > 0 && i < map->num_buckets) {
		entry = (map->entries)[i];

		if (!entryIsOpen(entry)) {
			map->deleteValue(entry->value);
			map->deleteKey(entry->key);
			free(entry);

			(map->entries)[i] = (HashEntry *)EMPTY_ENTRY;
			(map->length)--;
		}

		i++;
	}
}


void hashmapFree(HashMap *map) {
	if (map == NULL) {
		return;
	}

	hashmapClear(map);
	free(map->entries);
	free(map);
}


/*
 * The hash map can't be NULL before calling this function.
 *
 * Returns true if the hash map's load factor has exceeded the allowable amount (i.e. LOAD_FACTOR)
 * and needs to be resized.
 *
 * Returns false otherwise.
 *
 * There is a special case for particularly small hash maps which can only be created by
 * using the `hashmapNewBuckets` function with very small amounts of buckets. Recall that
 * there must be at least 1 empty space in the hash map or else the lookup will never terminate.
 *
 * For example, imagine that a hash map has exactly 2 buckets and only 1 bucket is filled.
 * The load factor (of 0.5) is within the allowable amount (of 0.67) so the hash map will not be resized,
 * but adding another element would completely fill the hash map. This case must also signal
 * that a resize is necessary, or else future lookups will infinite loop.
 *
 * This special case where an insertion would compromise the integrity of hash map lookups
 * is what the second condition represents.
 */
static inline bool hashmapNeedsResize(const HashMap *map) {
	if (((double)(map->length) / (double)(map->num_buckets)) > LOAD_FACTOR) {
		return true;
	}
	return map->length == map->num_buckets-1;
}


/*
 * Resizes the hash map so that it holds four times as many buckets as it did previously.
 * Once the hash map becomes sufficiently large (its number of buckets is at least HASHMAP_LARGE_SIZE)
 * the hash map is only resized to hold twice as many buckets as it did previously.
 *
 * All its entries are reinserted into the buckets after resizing, potentially receiving
 * a new position in the hash map thanks to the increase in open space and size.
 *
 * This is a pretty expensive operation that should performed as infrequently as possible.
 * This cost is part of the reason why these hash maps are resized to be so massive.
 *
 * Returns false if the hash map is NULL, or if any memory allocation fails.
 * Returns true otherwise, indicating a successful operation.
 */
static bool hashmapResize(HashMap *map) {
	printf("hashmapResize is unimplemented");
	return false;
}


bool hashmapInsert(HashMap *map, void *key, void *value) {
	if (map == NULL || key == NULL) {
		return false;
	}

	// Resize the hashmap
	if (hashmapNeedsResize(map)) {
		// hashmapResize returns false on an error
		if (!hashmapResize(map)) {
			// If the resize fails, then there's no sense in trying to continue
			return false;
		}
	}

	int64_t hashVal = map->hash(key);
	HashEntry *toInsert = hashentryNew(hashVal, key, value);
	if (toInsert == NULL) {
		// Memory allocation failure
		return false;
	}

	long i = labs(hashVal) % (map->num_buckets);

	// Find an empty spot in the hash map by linear probing
	while ((map->entries)[i] != EMPTY_ENTRY) {
		// TODO check if current entry has the same hash as what we're trying to insert.
		// If it does, then free the old value (dont have to free the entire entry) and replace it.
		// Make sure to not increment the hash map's length.

		// Wrap the index around the end of the array if necessary
		i = (i+1) % (map->num_buckets);	
	}

	(map->entries)[i] = toInsert;
	(map->length)++;

	return true;
}


void *hashmapGet(const HashMap *map, void *key) {
	if (map == NULL || key == NULL) {
		return NULL;
	}

	void *toReturn = NULL;
	int64_t hashVal = map->hash(key);
	long i = labs(hashVal) % (map->num_buckets);
	HashEntry *cur = (map->entries)[i];

	while (cur != EMPTY_ENTRY) {
		if (cur->hash == hashVal) {
			toReturn = cur->value;
			break;
		}

		// Wrap the index around the end of the array if necessary
		i = (i+1) % (map->num_buckets);
		cur = (map->entries)[i];
	}

	return toReturn;
}


void *hashmapRemove(HashMap *map, void *key) {
	if (map == NULL || key == NULL) {
		return NULL;
	}

	void *toReturn = NULL;
	int64_t hashVal = map->hash(key);
	long i = labs(hashVal) % (map->num_buckets);
	HashEntry *cur = (map->entries)[i];

	while (cur != EMPTY_ENTRY) {
		if (cur->hash == hashVal) {
			toReturn = cur->value;
			map->deleteKey(cur->key);
			free(cur);
			(map->entries)[i] = (HashEntry *)DUMMY_ENTRY;
			(map->length)--;
			break;
		}

		// Wrap the index around the end of the array if necessary
		i = (i+1) % (map->num_buckets);
		cur = (map->entries)[i];
	}

	return toReturn;
}


bool hashmapDeleteKey(HashMap *map, void *key) {
	if (map == NULL) {
		return false;
	}

	void *value = hashmapRemove(map, key);
	if (value == NULL) {
		return false;
	}

	map->deleteValue(value);
	return true;
}


bool hashmapContains(const HashMap *map, void *key) {
	if (map == NULL) {
		return false;
	}

	int64_t hashVal = map->hash(key);
	long i = labs(hashVal) % (map->num_buckets);
	HashEntry *cur = (map->entries)[i];

	while (cur != EMPTY_ENTRY) {
		if (cur != DUMMY_ENTRY && cur->hash == hashVal) {
			return true;
		}

		// Wrap the index around the end of the array if necessary
		i = (i+1) % (map->num_buckets);
		cur = (map->entries)[i];
	}

	return false;
}


long hashmapLength(const HashMap *map) {
	if (map == NULL) {
		return -1;
	}
	return map->length;
}


bool hashmapIsEmpty(const HashMap *map) {
	printf("hashmapIsEmpty is unimplemented\n");
	return false;
}


char *hashmapValueToString(const HashMap *map, void *value) {
	printf("hashmapValueToString is unimplemented\n");
	return NULL;
}


void hashmapPrintValue(const HashMap *map, void *value) {
	printf("hashmapPrintValue is unimplemented\n");
}


char *hashmapToString(const HashMap *map) {
	size_t length = 1;	// starting length of 1 for opening '{'
	char *toReturn;
	char *keyStr;
	char *valueStr;

	if (map == NULL || map->length == 0) {
		toReturn = malloc(length+2);	// +2 for closing '}' and null terminator
		return strcpy(toReturn, "{}");
	}

	toReturn = malloc(length+1);	// +1 for null terminator
	strcpy(toReturn, "{");
	HashEntry *entry;

	for (long i = 0; i < map->num_buckets; i++) {
		entry = (map->entries)[i];
		if (entryIsOpen(entry)) {
			continue;
		}

		keyStr = map->printKey(entry->key);
		valueStr = map->printValue(entry->value);

		// +4 for a ": " to separate key and value and ", " to separate entries
		length += strlen(keyStr) + strlen(valueStr) + 4;

		toReturn = realloc(toReturn, length+1);	// +1 for null terminator
		strcat(toReturn, keyStr);
		strcat(toReturn, ": ");
		strcat(toReturn, valueStr);
		strcat(toReturn, ", ");

		free(keyStr);
		free(valueStr);
	}

	// Replace the trailing ", " with just "}"
	toReturn[length-1] = '\0';
	toReturn[length-2] = '}';

	return toReturn;
}


void hashmapPrint(const HashMap *map) {
	char *toPrint = hashmapToString(map);
	printf("%s\n", toPrint);
	free(toPrint);
}


char *__hashmapToStringDEBUG(const HashMap *map) {
	size_t length = 1;	// starting length of 1 for opening '{'
	char *toReturn;
	char *keyStr;
	char *valueStr;

	if (map == NULL || map->length == 0) {
		toReturn = malloc(length+2);	// +2 for closing '}' and null terminator
		return strcpy(toReturn, "{}");
	}

	toReturn = malloc(length+1);	// +1 for null terminator
	strcpy(toReturn, "{");
	HashEntry *entry;

	for (long i = 0; i < map->num_buckets; i++) {
		entry = (map->entries)[i];
		if (entry == EMPTY_ENTRY) {
			toReturn = realloc(toReturn, length+10);
			strcat(toReturn, "<EMPTY>, ");
			length += 9;
			continue;
		} else if (entry == DUMMY_ENTRY) {
			toReturn = realloc(toReturn, length+10);
			strcat(toReturn, "<DUMMY>, ");
			length += 9;
			continue;
		}

		keyStr = map->printKey(entry->key);
		valueStr = map->printValue(entry->value);

		// +4 for a ": " to separate key and value and ", " to separate entries
		length += strlen(keyStr) + strlen(valueStr) + 4;

		toReturn = realloc(toReturn, length+1);	// +1 for null terminator
		strcat(toReturn, keyStr);
		strcat(toReturn, ": ");
		strcat(toReturn, valueStr);
		strcat(toReturn, ", ");

		free(keyStr);
		free(valueStr);
	}

	// Replace the trailing ", " with just "}"
	toReturn[length-1] = '\0';
	toReturn[length-2] = '}';

	return toReturn;
}

void __hashmapPrintDEBUG(const HashMap *map) {
	char *toPrint = __hashmapToStringDEBUG(map);
	printf("%s\n", toPrint);
	free(toPrint);
}

