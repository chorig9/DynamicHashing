#pragma once
#include <functional>
#include "Storage.h"
#include "RecordPageRuntime.h"
#include "Directory.h"
#include "Allocator.h"

uint32_t FNV1(const uint8_t*const pBytes, const size_t pSize);
uint32_t FNV1hash(int key);
inline uint32_t none(int key) { return key; };

class Hashing
{
public:
	Hashing(std::string filename, std::function<uint32_t(int)> hash = FNV1hash, bool show = false);

	void insertRecord(Record record);

	void removeRecord(int key);

	Record getRecord(int key);

	bool updateRecord(Record record);

	void show();

	const std::function<uint32_t(int)> hash;

private:
	RecordPageRuntime readRecordsPage(uint32_t ptr);

	bool showInfo;

	Storage recordsIO;

	Directory directory;

	Allocator allocator;
};
