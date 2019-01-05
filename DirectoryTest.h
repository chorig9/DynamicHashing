#pragma once
#include "TestUtils.h"
#include <iostream>
#include <cassert>

namespace DirectoryTest
{

	void test1()
	{
			{
				Directory dir("test-directory");

				dir[0].pagePtr = 1;
				assert(equals(showDirectory(dir), { 1 }));

				dir.extend();
				dir.extend();
				assert(equals(showDirectory(dir), { 1, 1, 1, 1 }));

				dir[2].pagePtr = 2;
				assert(equals(showDirectory(dir), { 1, 1, 2, 1 }));

				dir.extend();
				assert(equals(showDirectory(dir), { 1, 1, 1, 1, 2, 2, 1, 1 }));
			}

			std::remove("test-directory");
	}

	void test2()
	{
		{
			Directory dir("test2-directory");

			dir.extend();
			dir.extend();
			dir.extend();

			dir[0].pagePtr = 0;
			dir[1].pagePtr = 0;
			dir[2].pagePtr = 1;
			dir[3].pagePtr = 2;
			dir[4].pagePtr = 3;
			dir[5].pagePtr = 3;
			dir[6].pagePtr = 3;
			dir[7].pagePtr = 3;

			dir.extend();

			assert(equals(showDirectory(dir), {
				0, 0, 0, 0,
				1, 1,
				2, 2,
				3, 3, 3, 3, 3, 3, 3, 3 }));
		}

		std::remove("test2-directory");
	}

	void runTests()
	{
		test1();
		test2();
	}
}
