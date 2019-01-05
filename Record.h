#pragma once

struct Record
{
	Record(int key = 0): key(key)
	{
		
	}

	static Record null()
	{
		return Record(0);
	}

	bool isNull()
	{
		return key == 0;
	}

	int key;

	char data[30];
};