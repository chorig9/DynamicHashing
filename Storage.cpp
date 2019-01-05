#include "Storage.h"

Storage::Storage(std::string filename)
 :filename(filename)
{
	file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
	if (!file)
	{
		file.open(filename, std::ios::out);
		file.close();

		file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
	}

	readPageUnbuffered(0, buffer.page);
	buffer.ptr = 0;
}

Storage::~Storage()
{
	flush();
}

std::string Storage::getFilename()
{
	return filename;
}

void Storage::readPage(uint32_t ptr, Page& page)
{
	ptr = addressRoundown(ptr);

	if (buffer.ptr != ptr)
	{
		writePageUnbuffered(buffer.ptr, buffer.page);

		buffer.ptr = ptr;
		readPageUnbuffered(ptr, buffer.page);
	}

	page = buffer.page;
}

void Storage::readPageUnbuffered(uint32_t ptr, Page& page)
{
	file.clear();
	file.seekg(ptr);
	file.read(page.data.bytes, PAGE_SIZE);

	if (file.gcount() != PAGE_SIZE)
		memset(page.data.bytes, 0, PAGE_SIZE);

	nRead++;
}

void Storage::writePageUnbuffered(uint32_t ptr, const Page& page)
{
	file.clear();
	file.seekp(ptr);
	file.write(page.data.bytes, PAGE_SIZE);
	file.flush();

	nWrote++;
}

void Storage::flush()
{
	writePageUnbuffered(buffer.ptr, buffer.page);
}

void Storage::writePage(uint32_t ptr, const Page& page)
{
	ptr = addressRoundown(ptr);

	if (buffer.ptr != ptr)
	{
		writePageUnbuffered(buffer.ptr, buffer.page);

		buffer.ptr = ptr;
	}

	buffer.page = page;
}

void Storage::resetCounters()
{
	nRead = nWrote = 0;
}

std::pair<int, int> Storage::getCounters()
{
	return{ nRead, nWrote };
}

void Storage::remove()
{
	swap(file, std::fstream("dymmyname"));	
	std::remove(filename.c_str());
}
