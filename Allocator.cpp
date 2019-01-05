#include "Allocator.h"

Allocator::Allocator(Storage& storage):
	storage(storage), allocatorStorage(storage.getFilename() + "-allocator")
{
	Page tmp;
	allocatorStorage.readPage(0, tmp);

	page = tmp.data.allocator;
}

Allocator::~Allocator()
{
	Page tmp;
	tmp.data.allocator = page;

	allocatorStorage.writePage(0, tmp);
}

RecordPageRuntime Allocator::getNewPage()
{
	page.nAllocated++;

	if (page.firstFree == NULL)
	{
		RecordPageRuntime recordsPage(page.nUsed * PAGE_SIZE);
		storage.readPage(recordsPage.getAddress(), *recordsPage);

		page.nUsed++;

		return recordsPage;
	}

	auto freeSlot = page.firstFree - 1;

	RecordPageRuntime recordsPage(freeSlot * PAGE_SIZE);
	storage.readPage(recordsPage.getAddress(), *recordsPage);

	page.firstFree = recordsPage->nextFree;

	return recordsPage;
}

void Allocator::free(uint32_t address)
{
	page.nAllocated--;

	auto nextFree = page.firstFree;

	page.firstFree = (address / PAGE_SIZE) + 1;

	Page page;
	storage.readPage(address, page);

	page.data.records.nextFree = nextFree;

	storage.writePage(address, page);
}
