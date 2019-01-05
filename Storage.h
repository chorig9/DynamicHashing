#pragma once
#include <string>
#include <fstream>
#include "Layout.h"
#include "utils.h"

class Storage
{
public:
	Storage(std::string filename);

	~Storage();

	std::string getFilename();

	void readPage(uint32_t ptr, Page& page);

	void writePage(uint32_t ptr, const Page& page);

	void readPageUnbuffered(uint32_t ptr, Page& page);

	void writePageUnbuffered(uint32_t ptr, const Page& page);

	void flush();

	void remove();

	void resetCounters();

	std::pair<int, int> getCounters();

private:
	std::string filename;

	std::fstream file;

	Buffer buffer;

	int nRead = 0, nWrote = 0;
};
