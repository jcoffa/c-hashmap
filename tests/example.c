#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HashMap.h"

typedef struct {
	uint16_t value;
	char *name;
} Foo;

char *strdup2(char *);
Foo *newFoo(uint16_t, const char *);
void deleteFoo(void *);
char *fooToStr(void *);
void deleteStr(void *);
char *strToStr(void *);
void printFoo(Foo *);

int main(void) {
	Foo *foo;
	bool ret;
	long oldLength;

	/*********
	 * SETUP *
	 *********/
	puts("");
	puts("STARTING SETUP");
	puts("==============\n");

	puts("Creating hashmap with 4 starting buckets");
	HashMap *map = hashmapNewBuckets(4, DEFAULT_HASH, deleteFoo, fooToStr, deleteStr, strToStr);
	printf("Done. Success? %s\n", map ? "yes" : "no");
	puts("");
	if (!map) {
		fprintf(stderr, "Hash map could not be created. Tests will not be run.\n");
		exit(1);
	}


	/*************
	 * INSERTION *
	 *************/
	puts("STARTING INSERTION TESTS");
	puts("========================\n");

	// Fill up the hash map with a few values

	printf("Inserting value Foo<5, \"number 5\"> with key \"5\"");
	ret = hashmapInsert(map, strdup2("5"), newFoo(5, "number 5"));
	printf("; Success? %s\n", ret ? "yes" : "no");
	printf("Hashmap after insertion (length=%li): ", hashmapLength(map));
	__hashmapPrintDEBUG(map);
	puts("");

	printf("Inserting value Foo<20000, \"number 20000\"> with key \"20000\"");
	ret = hashmapInsert(map, strdup2("20000"), newFoo(20000, "number 20000"));
	printf("; Success? %s\n", ret ? "yes" : "no");
	printf("Hashmap after insertion (length=%li): ", hashmapLength(map));
	__hashmapPrintDEBUG(map);
	puts("");

	printf("Inserting value Foo<12345, \"number 12345\"> with key \"12345\"");
	ret = hashmapInsert(map, strdup2("12345"), newFoo(12345, "number 12345"));
	printf("; Success? %s\n", ret ? "yes" : "no");
	printf("Hashmap after insertion (length=%li): ", hashmapLength(map));
	__hashmapPrintDEBUG(map);
	puts("");

	puts("(Hash map should be resized here)");
	printf("Inserting value Foo<42069, \"number 42069\"> with key \"42069\"");
	ret = hashmapInsert(map, strdup2("42069"), newFoo(42069, "number 42069"));
	printf("; Success? %s\n", ret ? "yes" : "no");
	printf("Hashmap after insertion (length=%li): ", hashmapLength(map));
	__hashmapPrintDEBUG(map);
	puts("");

	printf("Inserting value Foo<333, \"number 333\"> with key \"333\"");
	ret = hashmapInsert(map, strdup2("333"), newFoo(333, "number 333"));
	printf("; Success? %s\n", ret ? "yes" : "no");
	printf("Hashmap after insertion (length=%li): ", hashmapLength(map));
	__hashmapPrintDEBUG(map);
	puts("");

	printf("Inserting value Foo<6789, \"number 6789\"> with key \"6789\"");
	ret = hashmapInsert(map, strdup2("6789"), newFoo(6789, "number 6789"));
	printf("; Success? %s\n", ret ? "yes" : "no");
	printf("Hashmap after insertion (length=%li): ", hashmapLength(map));
	__hashmapPrintDEBUG(map);
	puts("");

	printf("Inserting value Foo<9876, \"number 9876\"> with key \"9876\"");
	ret = hashmapInsert(map, strdup2("9876"), newFoo(9876, "number 9876"));
	printf("; Success? %s\n", ret ? "yes" : "no");
	printf("Hashmap after insertion (length=%li): ", hashmapLength(map));
	__hashmapPrintDEBUG(map);
	puts("");

	printf("Inserting value Foo<4198, \"number 4198\"> with key \"4198\"");
	ret = hashmapInsert(map, strdup2("4198"), newFoo(4198, "number 4198"));
	printf("; Success? %s\n", ret ? "yes" : "no");
	printf("Hashmap after insertion (length=%li): ", hashmapLength(map));
	__hashmapPrintDEBUG(map);
	puts("");


	// Testing invalid insertions
	foo = newFoo(0, "this won't be inserted");

	// Insert NULL key
	puts("Inserting value with NULL key");
	ret = hashmapInsert(map, NULL, foo);
	printf("; Success? %s\n", ret ? "yes" : "no");
	printf("Hashmap after insertion (length=%li): ", hashmapLength(map));
	__hashmapPrintDEBUG(map);
	puts("");

	// Insert NULL hash map
	puts("Inserting value and key into NULL hash map");
	ret = hashmapInsert(NULL, "this won't be inserted", foo);
	printf("; Success? %s\n", ret ? "yes" : "no");
	printf("Hashmap after insertion (length=%li): ", hashmapLength(map));
	__hashmapPrintDEBUG(map);
	puts("");

	// Don't neeed the uninserted Foo struct anymore
	deleteFoo(foo);


	// Overwriting a value already in the hash map by inserting with the same key
	printf("Overwriting value with key 5 with new value Foo<5, \"the cooler 5\">");
	oldLength = hashmapLength(map);
	ret = hashmapInsert(map, strdup2("5"), newFoo(5, "the cooler 5"));
	printf("; Success? %s; Is length unchanged? %s\n", \
			ret ? "yes" : "no", \
			oldLength == hashmapLength(map) ? "yes" : "no");
	printf("Hashmap after insertion (length=%li): ", hashmapLength(map));
	__hashmapPrintDEBUG(map);
	puts("");

	puts("DONE INSERTION TESTS\n");


	/*****************
	 * VALUE LOOKKUP *
	 *****************/
	puts("STARTING LOOKUP TESTS");
	puts("=====================\n");

	puts("Hashmap before lookup tests:");
	hashmapPrint(map);
	puts("");

	printf("Getting value with key \"5\": ");
	foo = hashmapGet(map, "5");
	printFoo(foo);
	puts("");

	printf("Getting value with key \"4198\": ");
	foo = hashmapGet(map, "4198");
	printFoo(foo);
	puts("");

	printf("Getting value with key \"333\": ");
	foo = hashmapGet(map, "333");
	printFoo(foo);
	puts("");


	// Invalid lookup tests
	printf("Getting value with key \"25\" (which is not in the hash map): ");
	foo = hashmapGet(map, "25");
	printFoo(foo);
	puts("");

	printf("Getting value with key NULL (which is not in the hash map): ");
	foo = hashmapGet(map, NULL);
	printFoo(foo);
	puts("");

	printf("Getting value from NULL hash map: ");
	foo = hashmapGet(NULL, "5");
	printFoo(foo);
	puts("");

	puts("DONE LOOKUP TESTS\n");


	/***********
	 * REMOVAL *
	 ***********/
	puts("STARTING REMOVAL TESTS");
	puts("======================\n");

	printf("Hashmap before any removals (length=%li): ", hashmapLength(map));
	__hashmapPrintDEBUG(map);
	puts("");

	printf("Removing value with key \"5\": ");
	foo = hashmapRemove(map, "5");
	printFoo(foo);
	printf("Hashmap after removal (length=%li): ", hashmapLength(map));
	__hashmapPrintDEBUG(map);
	puts("");
	deleteFoo(foo);

	printf("Removing value with key \"6789\": ");
	foo = hashmapRemove(map, "6789");
	printFoo(foo);
	printf("Hashmap after removal (length=%li): ", hashmapLength(map));
	__hashmapPrintDEBUG(map);
	puts("");
	deleteFoo(foo);

	printf("Removing value with key \"4198\": ");
	foo = hashmapRemove(map, "4198");
	printFoo(foo);
	printf("Hashmap after removal (length=%li): ", hashmapLength(map));
	__hashmapPrintDEBUG(map);
	puts("");
	deleteFoo(foo);


	// Invalid removal tests
	printf("Removing value with key \"777\" (which isn't in the hash map): ");
	oldLength = hashmapLength(map);
	foo = hashmapRemove(map, "5");
	printFoo(foo);
	printf("Is length unchanged? %s\n", oldLength == hashmapLength(map) ? "yes" : "no");
	printf("Hashmap after removal (length=%li): ", hashmapLength(map));
	__hashmapPrintDEBUG(map);
	puts("");

	printf("Removing value with key NULL (which isn't in the hash map): ");
	oldLength = hashmapLength(map);
	foo = hashmapRemove(map, NULL);
	printFoo(foo);
	printf("Is length unchanged? %s\n", oldLength == hashmapLength(map) ? "yes" : "no");
	printf("Hashmap after removal (length=%li): ", hashmapLength(map));
	__hashmapPrintDEBUG(map);
	puts("");

	printf("Removing value from NULL hash map: ");
	oldLength = hashmapLength(map);
	foo = hashmapRemove(NULL, "20000");
	printFoo(foo);
	printf("Is length unchanged? %s\n", oldLength == hashmapLength(map) ? "yes" : "no");
	printf("Hashmap after removal (length=%li): ", hashmapLength(map));
	__hashmapPrintDEBUG(map);
	puts("");


	/************
	 * TEARDOWN *
	 ************/
	puts("STARTING TEARDOWN");
	puts("=================\n");

	puts("Freeing hash map");
	hashmapFree(map);
    return 0;
}


char *strdup2(char *str) {
	char *toReturn = malloc(strlen(str) + 1);
	return strcpy(toReturn, str);
}

Foo *newFoo(uint16_t value, const char * name) {
	Foo *toReturn = malloc(sizeof(Foo));

	toReturn->value = value;
	toReturn->name = malloc(strlen(name) + 1);
	strcpy(toReturn->name, name);

	return toReturn;
}

void deleteFoo(void *foo) {
	Foo *toFree = (Foo*)foo;
	free(toFree->name);
	free(toFree);
}

char *fooToStr(void *foo) {
	char *toReturn;
	Foo *toPrint = (Foo*)foo;

	// + 5 for 5 digits of uint16_t value
	// + 9 for extra characters to make the string pretty
	// + 1 for null terminator
	// ===
	//  15
	toReturn = malloc(strlen(toPrint->name) + 15);
	sprintf(toReturn, "Foo<%05d, \"%s\">", toPrint->value, toPrint->name);

	return toReturn;
}

void deleteStr(void *string) {
	free(string);
}

char *strToStr(void *str) {
	char *string = (char*)str;
	char *toReturn = malloc(strlen(string) + 1);
	strcpy(toReturn, string);

	return toReturn;
}

void printFoo(Foo *foo) {
	if (!foo) {
		puts("NULL");
		return;
	}

	char *toPrint = fooToStr(foo);
	puts(toPrint);
	free(toPrint);
}

