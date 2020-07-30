#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***********************
 * PREPROCESSOR MACROS *
 ***********************/

/*
 * The highest allowed ratio of (number of entries in hash map):(total buckets available)
 * before the hash map needs to be resized and the indices recalculated.
 *
 * With this current value, the hash map will be resized after two-thirds of its buckets are filled.
 */
#define LOAD_FACTOR (2.0 / 3.0)

// Default number of buckets in a new hash map
#define DEFAULT_BUCKETS (16)


/*
 * A hash map with this amount of buckets is considered "large" and grows at a reduced rate
 * compared to a "small" hash map.
 *
 * A "small" hash map is resized to contain 4 times as many buckets as it did previously,
 * while a "large" hash map is resized to only contain 2 times as many buckets as it did previously.
 */
#define HASHMAP_LARGE_SIZE (65536)	// = 2^16
#define HASHMAP_IS_LARGE(map) (map->num_buckets >= HASHMAP_LARGE_SIZE)


/*
 * Signals to the `hashmapNew` and `hashmapNewBuckets` functions to use the default hash function
 * of djb2 for string (char *) data instead of requiring the caller to provide their own hash function.
 */
#define DEFAULT_HASH NULL


/**************
 * STRUCTURES *
 **************/

/*
 * A key-value pair to be stored by the hash map.
 *
 * The key's full hash value is also stored so that it does not have to
 * be recalculated during lookup or whenever the hash map is resized.
 */
typedef struct hashMapEntry {
	int64_t hash;
	void *key;
	void *value;
} HashEntry;

/*
 * Metadata head of the hash map. 
 * Contains the function pointers for working with the abstracted hash map data.
 *
 * =======================================================================================
 *
 * My other attempt at writing a hash map took inspiration from CPython3.6 dict's,
 * but it proved too complicated for my tiny ape brain. This is a much simpler
 * and naive (but still quite effective) approach to implementing a hash map in C.
 *
 * The main difference between this implementation and the one used by Python
 * is memory usage; the Python version uses much less memory thanks to its
 * clever design choice of using an index array as the hash table which maps
 * to indices in a compact sequential array. This means the actual "hash table"
 * contains numbers that are only a couple bytes large instead of the comparatively
 * massive 24 byte HashEntry structure used by this implementation.
 *
 * Resizing the hash map is also slower than CPython's in this implementation,
 * and there may be more collisions due to the use of linear probing instead
 * of the more complex linear congruential generator sequence (5*i + perturb + 1)
 *
 * =======================================================================================
 *
 * This hash map uses the "open addressing" approach to collision handling, and uses a
 * naive "linear probing" method to handle collisions. This means that this hash map
 * implementation is somewhat dependant on having a good hash function to spread out
 * its elements.
 * Here's a pretty incredible Stack Exchange post on how some popular
 * hash functions perform with English words, numbers, and GUID's as their data:
 * 		https://softwareengineering.stackexchange.com/a/145633
 *
 * This implementation will resize and re-index its entries once its load factor exceeds 2/3;
 * quadrupling the size of the hash map each resize (until its number of buckets reaches
 * HASHMAP_LARGE_SIZE, at which point it is doubled each resize)
 */
typedef struct hashMapHead {
	HashEntry **entries;	// The key-value pairs stored in the hash map
	long length;			// The number of entries currently in the hash map
	long num_buckets;		// The maximum number of entries the hash map can hold.
	                 		// For a new hash map, this field is equal to DEFAULT_BUCKETS.

	int64_t (*hash)(void *);		// Hash function pointer to turn a key into an 8-byte signed integer
	void (*deleteValue)(void *);	// Function pointer to free a value in the hash map
	char *(*printValue)(void *);	// Function pointer to create a string from a value in the hash map
	void (*deleteKey)(void *);		// Function pointer to free a key in the hash map
	char *(*printKey)(void *);		// Function pointer to create a string from a key in the hash map
} HashMap;


/*************
 * FUNCTIONS *
 *************/

/*
 * Function to initialize the HashMap metadata head to the appropriate function pointers.
 * Allocates memory to the struct, unless any of the function pointers except `hash` are NULL.
 * If the `hash` function pointer is NULL, the djb2 hashing algorithm for strings (char *) is used.
 * Passing the literal NULL into a function isn't very descriptive and looks odd, so callers are
 * encouraged to use the DEFAULT_HASH macro instead in order to be explicit about their intentions.
 *
 * If any other function pointers are NULL, then NULL is returned instead and no memory is allocated.
 * NULL is also returned if any memory allocation fails.
 *
 * The HashMap provides an interface to a generic collection of keys each mapped to a generic value.
 * The first function pointer is the hash function used to turn keys into 8-byte signed integer hashes
 * in typical hash map fashion. The four remaining function pointers allow the struct to delete
 * its keys and values and convert them into a readable string format.
 * The (void *) arguments are to be casted into their proper data type (i.e. whatever
 * data type the hash map's keys and values will be) and do as follows:
 *
 * 	void deleteVal(void *toDelete) : free all memory associated with the value `toDelete`
 * 	char *printVal(void *toPrint)  : return a string representation of the value `toPrint`
 * 	void deleteKey(void *toDelete) : free all memory associated with the key `toDelete`
 * 	char *printKey(void *toPrint)  : return a string representation of the key `toPrint`
 *
 * Examples of these functions are provided for string (char *) data in the README.
 */
HashMap *hashmapNew(int64_t (*hash)(void *), void (*deleteValue)(void *), char *(*printValue)(void *), \
        void (*deleteKey)(void *), char *(*printKey)(void *));


/*
 * Similar to the `hashmapNew` function, except the number of starting buckets can be given manually.
 * Useful for when you are guaranteed to need a large hash map and the default value will cause
 * a resize or two that could be avoided with the use of this function.
 *
 * The starting number of buckets, `B`, will be converted to the smallest power of two, `N`,
 * such that `B <= N`. For example, if 20 is given as the starting number of buckets, the created
 * hash map will actually have 32 buckets instead.
 */
HashMap *hashmapNewBuckets(long num_buckets, int64_t (*hash)(void *), void (*deleteValue)(void *), \
        char *(*printValue)(void *), void (*deleteKey)(void *), char *(*printKey)(void *));


/*
 * Frees every key and value in the hash map without deleting the hash map itself.
 * The hash map is NOT resized after removing all its entries. This could potentially
 * waste lots of memory if the hash map was decently large before it was cleared.
 */
void hashmapClear(HashMap *map);


/*
 * Frees all memory associated with the hash map, including the hash map itself.
 */
void hashmapFree(HashMap *map);


/*
 * Sets the value associated with the given key within the hash map,
 * replacing (and freeing) the data that was previously associated with that key if necessary.
 *
 * Returns false if the hash map is NULL, if any memory allocation fails, or if the key is NULL.
 * Hash map values are allowed to be NULL, but keys are not.
 *
 * Returns true otherwise, indicating a successful operation.
 */
bool hashmapInsert(HashMap *map, void *key, void *value);


/*
 * Returns the value associated with the given key.
 *
 * Returns NULL if the hash map itself is NULL, or if the key is not present in the hash map.
 */
void *hashmapGet(const HashMap *map, void *key);


/*
 * Returns and removes the value associated with the given key.
 * The key is also removed from the hash map.
 *
 * Returns NULL if the hash map itself is NULL, or if the key is not present in the hash map.
 */
void *hashmapRemove(HashMap *map, void *key);


/*
 * Removes the value associated with the given key and the key itself from the hash map.
 * Both the key and value are freed after removal.
 *
 * Returns false if the hash map is NULL, or if the key is not present in the hash map.
 *
 * Returns true otherwise, indicating a successful deletion.
 *
 * NOTE: it's called `hashmapDeleteKey` and not just `hashmapDelete` because I thought that sounded
 * too much like the `hashmapClear` and `hashmapFree` functions and might be confusing.
 */
bool hashmapDeleteKey(HashMap *map, void *key);


/*
 * Returns true if the given key is mapped to a value inside the hash map, and false otherwise.
 *
 * False is also returned when the hash map is a NULL pointer.
 */
bool hashmapContains(const HashMap *map, void *key);


/*
 * Returns the number of key-value pair entries currently stored in the hash map.
 *
 * If the hash map is NULL, -1 is returned.
 */
long hashmapLength(const HashMap *map);


/*
 * Returns true if the hash map contains no elements, and false otherwise.
 *
 * False is also returned when the hash map is a NULL pointer.
 */
bool hashmapIsEmpty(const HashMap *map);


/*
 * Returns a string representing the value associated with the given key,
 * using the hash map's `printVal` function pointer to create the string.
 *
 * If either the hash map or key is NULL, or if the key was not found in
 * the hash map, then a 1-character string containing only the null terminator
 * is returned instead.
 *
 * The string must be freed by the calling function after use.
 */
char *hashmapValueToString(const HashMap *map, void *key);


/*
 * A convenient alias for printing the string returned by `hashmapValuetoString(map, key)`
 * and then freeing the string that was created after printing it.
 * A newline is printed after the string is done printing.
 */
void hashmapPrintValue(const HashMap *map, void *key);


/*
 * Returns a string representing the entire hash map using it's `printKey` and `printVal`
 * function pointers to create the string.
 *
 * The string must be freed by the calling function after use.
 */
char *hashmapToString(const HashMap *map);


/*
 * A convenient alias for printing the string returned by `hashmapToString(map)`
 * and then freeing the string that was created after printing it.
 * A newline is printed after the string is done printing.
 */
void hashmapPrint(const HashMap *map);

#endif	// HASHMAP_H

