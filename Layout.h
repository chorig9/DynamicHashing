#pragma once
#include "Record.h"
#include <cstdint>

const uint32_t PAGE_SIZE = 1 << 7;

struct DirectoryEntry
{
	uint32_t pagePtr;

	int pageDepth;
};

const int DIRECTORY_ENTRIES_PER_PAGE = PAGE_SIZE / (sizeof(uint32_t) + sizeof(int));
static_assert(DIRECTORY_ENTRIES_PER_PAGE % 2 == 0, "DIRECTORY_ENTRIES_PER_PAGE");

struct DirectoryPage
{
	DirectoryEntry entry[DIRECTORY_ENTRIES_PER_PAGE];
};

struct FirstDirectoryPage
{
	int depth;
};

static_assert(sizeof(FirstDirectoryPage) <= PAGE_SIZE, "FirstDirectoryPage");

const int RECORDS_PER_PAGE = 2;//(PAGE_SIZE - 2 * sizeof(int)) / sizeof(Record);

struct RecordPage
{
	int nextFree;

	Record record[RECORDS_PER_PAGE];
};

struct AllocatorPage
{
	int nUsed;

	int firstFree;

	int nAllocated;
};

struct Page
{
	union X
	{
		X() {};

		AllocatorPage allocator;
		FirstDirectoryPage firstDir;
		DirectoryPage dir;
		RecordPage records;
		char bytes[PAGE_SIZE];
	} data;
};