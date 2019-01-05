#pragma once
#include <cstdint>
#include "Storage.h"
#include "utils.h"
#include <memory>

class Directory
{
public:
	Directory(std::string filename, bool initialize = true);

	~Directory();

	uint32_t getDepth();

	DirectoryEntry& operator[](int index);

	void extend();

	void halve();

	int getMaxPageDepth();

	void resetCounters();

	std::pair<int, int> getCounters();

private:
	void readPage(uint32_t ptr, Buffer& buffer);

	void flush();

	Buffer buffer;

	bool initialize;

	std::unique_ptr<Storage> storage;

	int depth;

	int nRead = 0, nWrote = 0;
};
