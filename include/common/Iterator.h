#pragma once
/**
 * Iterator.h
 */

/**
 * public accessor
 */
typedef struct _Iterator Iterator;
typedef struct _Iterator_protected Iterator_protected;
struct _Iterator {
	Iterator* (*prev)(Iterator*);
	Iterator* (*next)(Iterator*);
	void* (*data)(Iterator*);
	Iterator_protected* pro;
};

/**
 * Constructor
 */
Iterator* new_Iterator(void*);

/**
 * Destractor
 */
void delete_Iterator(Iterator**);

