- [Hash Maps Are Awesome](#hash-maps-are-awesome)
- [Building](#building)
- [Usage](#usage)
  - [Create a New Hash Map](#create-a-new-hash-map)
    - [The hash function](#the-hash-function)
    - [The deleteValue function](#the-deletevalue-function)
    - [The printValue function](#the-printvalue-function)
    - [The deleteKey function](#the-deletekey-function)
    - [The printKey function](#the-printkey-function)
    - [Putting it all together](#putting-it-all-together)
  - [Inserting Entries](#inserting-entries)
  - [Removing Entries](#removing-entries)
  - [Membership Testing](#membership-testing)
  - [Getting Values](#getting-values)
  - [String Conversion and Printing](#string-conversion-and-printing)
  - [Freeing the Hash Map](#freeing-the-hash-map)

---

<a name="hash-maps-are-awesome"></a>
## Hash Maps Are Awesome

... so it's unfortunate that C's standard library doesn't provide any data structures. I wanted to
fix that! This library provides a hash map structure that supports a number of features:
- stores generic keys and values
- allows for custom hash functions; write your own hash function to tailor hashes to the data
- automatic memory management and resizing (the actual structure must be freed manually, however)
- print the entire map with one function call

<a name="building"></a>
## Building
As easy as entering the cloned repo and running `make`. Because of how I setup my own directory
structure (I primarily made this library for my own personal use) the library is placed in the
directory _above_ the cloned repo. If this annoys you, just run `make hashmap` instead and the
library will be placed in the `bin` directory within the project tree.

<a name="usage"></a>
## Usage
Not every function will be covered in this section, but it'll be enough to start using the hash map.

Since all created hash maps are generic, all of its key and value arguments are of type `void *`.

<a name="create-a-new-hash-map"></a>
### Create a New Hash Map
To create a hash map, use `hashmapNew` with the following signature:

```c
HashMap *hashmapNew(int64_t (*hash)(void *), \
                    void (*deleteValue)(void *), char *(*printValue)(void *), \
                    void (*deleteKey)(void *), char *(*printKey)(void *));
```

... or alternatively, to manually specify the starting number of buckets in the hash map instead of
using the library's default value of 16:

```c
HashMap *hashmapNew(long numBuckets, int64_t (*hash)(void *), \
                    void (*deleteValue)(void *), char *(*printValue)(void *), \
                    void (*deleteKey)(void *), char *(*printKey)(void *));
```

These may look complicated but it's really not so bad once you know what it all means. Since all
created hash maps are generic, you have to write 4 (or 5 if you want to roll in your own hash
function) in order to allow the hash map to free and print its keys and values. All of the functions
use `void` pointer arguments since all of its data is generic.

In order to help describe how these functions should be written, the following structure will be
used as the values stored by the hash map (its keys will be strings):

```c
typedef struct foo {
    uint16_t value;
    char *name;
} Foo;
```

To create this Foo structure, the following function is used as an example:

```c
Foo *newFoo(uint16_t value, char *name) {
    Foo *toReturn = malloc(sizeof(Foo));

    toReturn->value = value;
    toReturn->name = malloc(strlen(name) + 1);
    strcpy(toReturn->name, name);

    return toReturn;
}
```

<a name="the-hash-function"></a>
#### The hash function

The first argument `hash` is a function pointer that takes a generic key and hashes it into a 64-bit
signed integer. If you're using string (`char *`) data as your strings, then you can pass the
`DEFAULT_HASH` macro instead to use the library's default string hashing algorithm; djb2.

If you aren't using string data, then you'll have to write this function yourself alongside the 4
other functions. Cast the `void *` argument into the data type of your key, and then perform your
hashing and return the 64-bit signed integer.

The default hash function, djb2, is provided here as an example:

```c
int64_t djb2(void *key) {
    char *str = (char *)key;
    int64_t hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) ^ c;    // (hash * 33) XOR c
    }
    return hash;
}
```

<a name="the-deletevalue-function"></a>
#### The deleteValue function

The second argument `deleteValue` is a function pointer that takes a generic value and frees all
memory associated with it. Using the example Foo struct, a `deleteFoo` function may look as follows:

```c
void deleteFoo(void *value) {
    Foo *toFree = (Foo *)value;

    free(toFree->name);
    free(toFree);
}
```

<a name="the-printvalue-function"></a>
#### The printValue function

The third argument `printValue` is a function pointer that takes a value from the hash map, turns
it into a string, and returns it. You'll need to allocate memory using `malloc` or the like in order
to return the string; any strings local to the function will become invalid once the function
returns. The function does not actually do any printing (bit of an unfortunate name). Using the
example Foo struct, a `printFoo` function may look as follows:

```c
char *printFoo(void *value) {
    Foo *toPrint = (Foo *)value;

    // +5 because a uint16_t can have at most 5 digits (0 to 65535)
    // +9 for the extra Foo(, "") characters to make the string look nicer
    // +1 for null terminator
    // = +15
    size_t length = strlen(toPrint->name) + 15;
    char *toReturn = malloc(length);

    snprintf(toReturn, length, "Foo(%0u, \"%s\")", toPrint->value, toPrint->name);
    return toReturn;
}
```

<a name="the-deletekey-function"></a>
#### The deleteKey function

The fourth argument `deleteKey` is a function pointer that takes a generic key and frees all memory
associated with it. This example hash map uses strings as keys, so a `deleteStr` may look as follows:

```c
void deleteStr(void *key) {
    char *toFree = (char *)key;
    free(toFree);
}
```

<a name="the-printkey-function"></a>
#### The printKey function

The fifth and final argument `printKey` is a function pointer that takes a generic key from the hash
map, turns it into a string, and returns it. You'll need to allocate memory using `malloc` or the
like in order to return the string; any strings local to the function will become invalid once the
function returns. The function does not actually do any printing (bit of an unfortunate name). This
example hash map uses strings as keys already, so this function is quite simple to imagine:

```c
char *printStr(void *key) {
    return (char *)key;
}
```


<a name="putting-it-all-together"></a>
#### Putting it all together

Now that all of the necessary functions are written (it wasn't even that difficult!) the hash map
can be created. The final call, using all our example functions and the library's default hash
function for string (`char *`) keys, would look like this:

```c
HashMap *map = hashmapNew(DEFAULT_HASH, deleteFoo, printFoo, deleteStr, printStr);
```

A hash map created this way will start with 16 buckets to place its data into. The hashmap resizes
itself to gain more buckets once its load factor (the ratio of filled buckets to total buckets)
exceeds `2/3`. If you know that you're going to need more (or less) buckets than that, you can use
`hashmapNewBuckets` instead to manually define the starting amount of buckets:

```c
// Create a hash map with 64 empty buckets
HashMap *map = hashmapNewBuckets(64, DEFAULT_HASH, deleteFoo, printFoo, deleteStr, printStr);

// Insert a bunch of entries without having to worry about resize overhead ...
```

---

**Note**: The amount of buckets will automatically change to the smallest power of two that is equal to or
greater than the amount of buckets you specified. For example, `hashmapNewBuckets(24, ...)` will
create a hash map with 32 empty buckets (since 32 = 2<sup>5</sup> is the smallest power of 2 that
is >= 24)

---


<a name="inserting-entries"></a>
### Inserting Entries

Now that a hash map has been created, we can insert key-value pair entries into it using the
appropriately named function `hashmapInsert`, which has the following signature:

```c
bool hashmapInsert(HashMap *map, void *key, void *value);
```

This function takes a hash map as its first argument, the key as its second, and the value as its
third. It returns `true` if the insertion was successful, and `false` if it wasn't. Failure cases
include:

- Memory allocation failed
- The map is `NULL`
- the key is `NULL` (the value is allowed to be `NULL`, however)

And here's an example of it in action:

```c
// Continuing with the example of a hashmap with string (char *) as keys
// and the example Foo struct as values.

// The key must be allocated manually so it doesn't get freed before we're done with it,
// and doesn't stick around longer than we need it to.
char *key = malloc(2);
strcpy(key, "2");

Foo *value = newFoo(2, "number 2");

hashmapInsert(map, key, value);

// Insert a new key and value
key = malloc(6);
strcpy(key, "12345");

hashmapInsert(key, newFoo(12345, "number 12345"));
```

---

**Note**: If the provided `key` is already present in the hash map when inserting, then it will be
overridden with the new data. The old value stored at this key will be freed and removed from the
hash map.

---


<a name="removing-entries"></a>
### Removing Entries

To remove an entry from the hashmap, us either the `hashmapRemove` or `hahsmapDeleteKey` function.
Their function signatures explain how they're different:

```c
void *hashmapRemove(HashMap *map, void *key);

bool hashmapDeleteKey(HashMap *map, void *key);
```

`hashmapRemove` will remove the entry with key `key` and return its associated value (or `NULL` if
the key was not found in the hash map).

`hashmapDeleteKey` will delete the entry with key `key` without returning its associated value. It
instead returns `true` if the entry was removed successfully, and `false` if the key was not found
in the hash map.

An example of these functions in use:

```c
// value will contain the value stored with key "2" if present in the map,
// or NULL otherwise
void *value = hashmapRemove(map, "2");

// ret will contain true if there was an entry with key "55" present in the map,
// or false otherwise
bool ret = hashmapDeleteKey(map, "55");
```


<a name="membership-testing"></a>
### Membership Testing

To determine whether a given key is present in the hash map, use `hashmapContains`. It's signature
is as follows:

```c
bool hashmapContains(const HashMap *map, void *key);
```

It returns `true` if the key `key` is present in the map, and `false` if it is not.

```c
// Continuing with the example of a hashmap with string (char *) as keys
// and the example Foo struct as values.
char *key = malloc(6);
strcpy(key, "44556");
hashmapInsert(map, key, newFoo(44556, "number 44556"));

// ret is now true
bool ret = hashmapContains(map, "44556");

// ret is now false
ret = hashmapContains(map, "this key is not in the hash map");
```


<a name="getting-values"></a>
### Getting Values

To retrieve values at a certain key in the hash map, use `hashmapGet`. It's signature is as follows:

```c
void *hashmapGet(const HashMap *map, void *key);
```

Example of its use:

```c
// Continuing with the example of a hashmap with string (char *) as keys
// and the example Foo struct as values.
char *key = malloc(3);
strcpy(key, "42");
hashmapInsert(map, key, newFoo(42, "number 42"));

// value now contains a pointer to the Foo object with value 42 and name "number 42"
Foo *value = hashmapGet(map, "42");
```

<a name="string-conversion-and-printing"></a>
### String Conversion and Printing

There are functions available to take values or the entire hash map and convert them to strings or
print them. They are as follows:

```c
// Converts the value associated with the key to a string.
// It must be freed by the caller after use.
char *hashmapValueToString(const HashMap *map, void *key);

// Prints the value associated with the key to stdout.
void hashmapPrintValue(const HashMap *map, void *key);

// Converts the entire hash map to a string.
// It must be freed by the caller after use.
char *hashmapToString(const HashMap *map);

// Prints the entire hash map to stdout.
void hashmapPrint(const HashMap *map);
```


<a name="freeing-the-hash-map"></a>
### Freeing the Hash Map

While all the internal memory is managed automatically, the hash map itself must be freed after it's
done being used. The function `hashmapDelete` does exactly this:

```c
void hashmapFree(HashMap *map);
```

Using it is equally as straightforward as its function signature:

```c
int main(void) {
    HashMap *map = hashmapNew(DEFAULT_HASH, deleteFoo, printFoo, deleteStr, printStr);

    // ... insert, get, remove elements, etc.

    hashmapFree(map);
    return 0;
}
```

