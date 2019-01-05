#pragma once
#include "TestUtils.h"
#include "Hashing.h"

namespace HashingTest
{
	Page readDirectory(Directory& dir, Storage& storage, int index)
	{
		Page page;

		storage.readPage(dir[index].pagePtr, page);

		return page;
	}

	void test1()
	{
		{
			Hashing hashing("test-hashing");

			Record r[] = { 1,2,3,4,5,6,7,8,9,10 };

			for (int i = 0; i < 10; i++)
			{
				hashing.insertRecord(r[i]);
				//hashing.show();
			}

			for (int i = 0; i < 10; i++)
				//std::cout << hashing.getRecord(r[i].key).key << std::endl;
				assert(hashing.getRecord(r[i].key).key == r[i].key);
		}

		std::remove("test-hashing-records");
		std::remove("test-hashing-directory");
		std::remove("test-hashing-records-allocator");
	}

	void test2()
	{
		{
			Hashing hashing("test-hashing");

			Record r[] = { 1,2,3,4,5,6,7,8,9,10 };

			for (int i = 0; i < 10; i++)
			{
				hashing.insertRecord(r[i]);
				//hashing.show();
			}
		}

		{
			Hashing hashing("test-hashing");

			Record r[] = { 1,2,3,4,5,6,7,8,9,10 };

			for (int i = 0; i < 10; i++)
				assert(hashing.getRecord(r[i].key).key == r[i].key);
		}

		std::remove("test-hashing-records");
		std::remove("test-hashing-directory");
		std::remove("test-hashing-records-allocator");
	}

	void test3()
	{
		{
			Hashing hashing("test-hashing", [](int key) {return key << 27; });
			Record r[] = { 2,1,4,10,20,16 };

			for (int i = 0; i < 6; i++)
			{
				hashing.insertRecord(r[i]);
				//hashing.show();
			}
		}
		{
			Hashing hashing("test-hashing", [](int key) {return key << 27; });
			Directory dir("test-hashing-directory");
			Storage records("test-hashing-records");
			Page p;

			assert(dir.getDepth() == 3);

			p = readDirectory(dir, records, 0);
			assert(dir[0].pageDepth == 3);
			assert(p.data.records.record[0].key == 2);
			assert(p.data.records.record[1].key == 1);

			p = readDirectory(dir, records, 1);
			assert(dir[1].pageDepth == 3);
			assert(p.data.records.record[0].key == 4);
			assert(p.data.records.record[1].key == 0);

			assert(dir[2].pagePtr == dir[3].pagePtr);
			p = readDirectory(dir, records, 2);
			assert(dir[2].pageDepth == 2);
			assert(p.data.records.record[0].key == 10);
			assert(p.data.records.record[1].key == 0);

			assert(dir[4].pagePtr == dir[5].pagePtr && dir[4].pagePtr == dir[6].pagePtr && dir[4].pagePtr == dir[7].pagePtr);
			p = readDirectory(dir, records, 4);
			assert(dir[4].pageDepth == 1);
			assert(p.data.records.record[0].key == 20);
			assert(p.data.records.record[1].key == 16);
		}
		{
			Hashing hashing("test-hashing", [](int key) {return key << 27; });
			hashing.removeRecord(2);
		}
		{
			Hashing hashing("test-hashing", [](int key) {return key << 27; });
			Directory dir("test-hashing-directory");
			Storage records("test-hashing-records");

			assert(dir.getDepth() == 2);
			auto p = readDirectory(dir, records, 0);
			assert(dir[0].pageDepth == 2);
			assert(p.data.records.record[0].key == 4);
			assert(p.data.records.record[1].key == 1);
		}

			std::remove("test-hashing-records");
			std::remove("test-hashing-directory");
			std::remove("test-hashing-records-allocator");
	}

	void runTests()
	{
		test1();
		test2();
		test3();
	}
}
