#pragma once
#include "Storage.h"
#include "RecordPageRuntime.h"

class Allocator
{
public:
	Allocator(Storage& storage);

	~Allocator();

	RecordPageRuntime getNewPage();

	void free(uint32_t address);

private:
	AllocatorPage page;

	Storage& storage;

	Storage allocatorStorage;
};
