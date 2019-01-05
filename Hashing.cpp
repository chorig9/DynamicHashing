#include "Hashing.h"
#include <iostream>
#include <cstdint>
#include <algorithm>

uint32_t FNV1(const uint8_t*const pBytes, const size_t pSize) {
	uint32_t rHash = 14695981039346656037;
	for (size_t i = 0; i<pSize; ++i) {
		rHash = (rHash^pBytes[i]) * 1099511628211;
	}
	return rHash;
}

uint32_t FNV1hash(int key)
{
	return FNV1((const uint8_t*)&key, 4);
}

Hashing::Hashing(std::string filename, std::function<uint32_t(int)> hash, bool showInfo)
	: recordsIO(filename + "-records"), directory(filename + "-directory"), allocator(recordsIO), hash(hash), showInfo(showInfo)
{
	if (directory.getDepth() == 0)
		directory[0] = { allocator.getNewPage().getAddress(), 0 };
}

void Hashing::show()
{
	for (int i = 0; i < (1 << directory.getDepth()); i++)
	{
		showBinary(i, directory.getDepth());
		std::cout << " %" << directory[i].pagePtr << std::endl;
	}

	std::cout << std::endl;

	bool bitmap[100] = { 0 };

	for (int i = 0; i < (1 << directory.getDepth()); i++)
	{
		if (bitmap[directory[i].pagePtr / PAGE_SIZE])
			continue;

		bitmap[directory[i].pagePtr / PAGE_SIZE] = true;

		Page page;
		recordsIO.readPage(directory[i].pagePtr, page);

		std::cout << "%" << directory[i].pagePtr << " depth: " << directory[i].pageDepth << std::endl;
		for (int j = 0; j < RECORDS_PER_PAGE; j++)
		{
			if (page.data.records.record[j].key == NULL)
			{
				std::cout << "NULL\n";
				continue;
			}

			std::cout << page.data.records.record[j].key << " ";
			showBinary(msBits(hash(page.data.records.record[j].key), directory[i].pageDepth), directory[i].pageDepth);
			std::cout << std::endl;
		}
	}

	std::cout << "-----------------------------" << std::endl;
	std::cout << std::endl;
}

void Hashing::insertRecord(Record record)
{
	recordsIO.resetCounters();
	directory.resetCounters();

	//if (!getRecord(record.key).isNull())
	//	return;

	auto hashKey = msBits(hash(record.key), directory.getDepth());
	auto recordsPage = readRecordsPage(directory[hashKey].pagePtr);

	while (recordsPage.isFull())
	{
		if (directory.getDepth() == directory[hashKey].pageDepth)
			directory.extend();

		auto targetPageBits = msBits(hash(record.key), directory[hashKey].pageDepth);

		auto oneBits = (targetPageBits << 1) | 1;   // targetPageBits with following 1
		auto nRemainingBits = (directory.getDepth() - directory[hashKey].pageDepth - 1);
		auto oneIndex = oneBits << nRemainingBits;

		// example: depth - 3, pageDepth - 1, 
		// ...
		// 100 -> key = 1...
		// 101 -> key = 1...
		// 110 -> key = 1...
		// 111 -> key = 1...
		// ...
		//
		// targetPageBits = 1 (bit used to index record)
		// oneBits = (1 << 1) | 1 = 1
		// nRemainginBits = 1
		//
		// so we have 11x <- x is remaining bit

		auto newPage = allocator.getNewPage();
		auto tranferRecordDecision = [&](Record& r) -> bool
		{
			return oneBit(hash(r.key), directory[hashKey].pageDepth + 1);
		};

		recordsPage.transferRecordsToNewPage(newPage, tranferRecordDecision);
		directory[hashKey].pageDepth++;

		// for every index in directory for which most significant bits are equal to 
		// most significant bits on targetPage and there is 1 after those bits
		for (auto i = oneIndex; i < oneIndex + (1 << nRemainingBits); i++)
			directory[i] = { newPage.getAddress(), directory[hashKey].pageDepth };

		// now, we have:
		// ...
		// 100 -> key = 10..
		// 101 -> key = 10..
		// 110 -> key = 11..
		// 111 -> key = 11..
		// ...

		hashKey = msBits(hash(record.key), directory.getDepth());
		if (directory[hashKey].pagePtr == recordsPage.getAddress())
		{
			recordsIO.writePage(newPage.getAddress(), *newPage);
			recordsIO.writePage(recordsPage.getAddress(), *recordsPage);
		}
		else
		{
			recordsIO.writePage(recordsPage.getAddress(), *recordsPage);
			recordsIO.writePage(newPage.getAddress(), *newPage);
		}

		recordsPage = readRecordsPage(directory[hashKey].pagePtr);
	}

	recordsPage.insertRecord(record);
	recordsIO.writePage(recordsPage.getAddress(), *recordsPage);

	if (showInfo)
	{
		std::cout << "nRead: " << recordsIO.getCounters().first << " nWrote: " << recordsIO.getCounters().second << std::endl;
		std::cout << "dir nRead " << directory.getCounters().first << " dir nWrote " << directory.getCounters().second << std::endl;
	}
}

void Hashing::removeRecord(int key)
{
	recordsIO.resetCounters();
	directory.resetCounters();

	auto index = msBits(hash(key), directory.getDepth());
	auto bucket = readRecordsPage(directory[index].pagePtr);

	bucket.removeRecord(key);
	recordsIO.writePage(bucket.getAddress(), *bucket);

	if (directory.getDepth() == 0)
		return;

	auto bucketDepth = directory[index].pageDepth;

	auto prefix = msBits(hash(key), bucketDepth);
	auto siblingKey = prefix ^ 1; // sibling differs in last bit only
	auto nRemainingBits = (directory.getDepth() - bucketDepth);
	auto siblingIndex = siblingKey << nRemainingBits;

	if (bucketDepth != directory[siblingIndex].pageDepth)
		return;

	auto siblingBucket = readRecordsPage(directory[siblingIndex].pagePtr);

	while (bucketDepth == directory[siblingIndex].pageDepth &&
		bucket.nRecords() + siblingBucket.nRecords() <= RECORDS_PER_PAGE)
	{
		for (int i = 0; i < RECORDS_PER_PAGE; i++)
		{
			if (!siblingBucket->record[i].isNull())
				bucket.insertRecord(siblingBucket->record[i]);
		}

		allocator.free(siblingBucket.getAddress());

		for (int i = siblingIndex; i < siblingIndex + (1 << nRemainingBits); i++)
			directory[i] = { bucket.getAddress(), bucketDepth - 1 };

		auto bucketIndex = prefix << nRemainingBits;
		for (int i = bucketIndex; i < bucketIndex + (1 << nRemainingBits); i++)
			directory[i].pageDepth--;

		recordsIO.writePage(bucket.getAddress(), *bucket);

		while (directory.getMaxPageDepth() < directory.getDepth())
			directory.halve();

		// recalculate all values
		index = msBits(hash(key), directory.getDepth());
		bucket = readRecordsPage(directory[index].pagePtr);
		bucketDepth = directory[index].pageDepth;
		prefix = msBits(hash(key), bucketDepth);
		siblingKey = prefix ^ 1; // sibling differs in last bit only
		nRemainingBits = (directory.getDepth() - bucketDepth);
		siblingIndex = siblingKey << nRemainingBits;
		siblingBucket = readRecordsPage(directory[siblingIndex].pagePtr);
	}

	if (showInfo)
	{
		std::cout << "nRead: " << recordsIO.getCounters().first << " nWrote: " << recordsIO.getCounters().second << std::endl;
		std::cout << "dir nRead " << directory.getCounters().first << " dir nWrote " << directory.getCounters().second << std::endl;
	}
}

Record Hashing::getRecord(int key)
{
	directory.resetCounters();
	recordsIO.resetCounters();

	auto hashKey = msBits(hash(key), directory.getDepth());
	auto page = readRecordsPage(directory[hashKey].pagePtr);

	if (showInfo)
	{
		std::cout << "nRead: " << recordsIO.getCounters().first << " nWrote: " << recordsIO.getCounters().second << std::endl;
		std::cout << "dir nRead " << directory.getCounters().first << " dir nWrote " << directory.getCounters().second << std::endl;
	}

	return page.getRecord(key);
}

bool Hashing::updateRecord(Record record)
{
	recordsIO.resetCounters();
	directory.resetCounters();

	auto hashKey = msBits(hash(record.key), directory.getDepth());
	auto page = readRecordsPage(directory[hashKey].pagePtr);

	auto r = page.updateRecord(record);

	recordsIO.writePage(page.getAddress(), *page);

	if (showInfo)
	{
		std::cout << "nRead: " << recordsIO.getCounters().first << " nWrote: " << recordsIO.getCounters().second << std::endl;
		std::cout << "dir nRead " << directory.getCounters().first << " dir nWrote " << directory.getCounters().second << std::endl;
	}

	return r;
}

RecordPageRuntime Hashing::readRecordsPage(uint32_t ptr)
{
	RecordPageRuntime records(ptr);

	recordsIO.readPage(ptr, *records);

	return records;
}
