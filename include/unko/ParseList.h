#pragma once
/**********************************************************
 *
 * ParseList is responsible for ...
 *
 **********************************************************/

#define IL_GROUPS	7
#define IL_NUMS		256

typedef struct InsertListStruct *InsertList;

typedef struct InsertListStruct {
	char* ngroup[IL_GROUPS][IL_NUMS];
} InsertListStruct;

typedef struct InsertListRangeStruct {
	int min;
	int max;
	uint32 empty;
} InsertListRangeStruct;

typedef struct InsertListGroupStruct {
	uint32 sa;
	InsertListRangeStruct *ranges;
} InsertListGroupStruct;
extern const char* const GrpName[];

bool ParseList(const char* listName, InsertList list);

