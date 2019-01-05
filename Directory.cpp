#include "Directory.h"
#include <cassert>

void Directory::readPage(uint32_t ptr, Buffer& buffer)
{
	ptr = addressRoundown(ptr);

	if (buffer.ptr == ptr && ptr != NULL)
		return;
	else if (buffer.ptr != NULL)
	{
		storage->writePage(buffer.ptr, buffer.page);
		nWrote++;
	}

	storage->readPage(ptr, buffer.page);
	nRead++;
	buffer.ptr = ptr;
}

void Directory::flush()
{
	storage->writePage(buffer.ptr, buffer.page);
	storage->flush();
	nWrote++;
}

Directory::Directory(std::string filename, bool initialize): initialize(initialize)
{
	storage = std::make_unique<Storage>(filename);

	if (!initialize)
		return;

	readPage(0, buffer);

	depth = buffer.page.data.firstDir.depth;
}

void Directory::resetCounters()
{
	nRead = nWrote = 0;
}

std::pair<int, int> Directory::getCounters()
{
	return{ nRead, nWrote };
}

int Directory::getMaxPageDepth()
{
	int max = 0;
	for (int i = 0; i < (1 << depth); i++)
		if ((*this)[i].pageDepth > max)
			max = (*this)[i].pageDepth;

	return max;
}

Directory::~Directory()
{
	if (!initialize)
		return;

	readPage(0, buffer);

	buffer.page.data.firstDir.depth = depth;

	flush();
}

uint32_t Directory::getDepth()
{
	return depth;
}

DirectoryEntry& Directory::operator[](int index)
{
	readPage((index / DIRECTORY_ENTRIES_PER_PAGE + 1) * PAGE_SIZE, buffer);

	return buffer.page.data.dir.entry[index % DIRECTORY_ENTRIES_PER_PAGE];
}

void copyFile(std::string srcn, std::string dstn)
{
	std::ifstream src(srcn, std::ios::binary);
	std::ofstream dest(dstn, std::ios::binary);
	dest << src.rdbuf();
}

void Directory::extend()
{
	flush();

	auto tmpFilename = storage->getFilename() + "-tmp";
	copyFile(storage->getFilename(), tmpFilename); // std::rename randomly fails

	auto oldDirectory = std::make_unique<Directory>(tmpFilename);

	depth++;
	for (int i = 0; i < (1 << (depth - 1)); i++)
	{
		(*this)[2 * i] = (*oldDirectory)[i];
		(*this)[2 * i + 1] = (*oldDirectory)[i];
	}

	flush();

	nRead += oldDirectory->nRead;
	nWrote += oldDirectory->nWrote;

	oldDirectory->storage->remove();
}

void Directory::halve()
{
	for (int i = 0; i < (1 << (depth - 1)); i++)
		(*this)[i] = (*this)[2 * i];

	depth--;
}
