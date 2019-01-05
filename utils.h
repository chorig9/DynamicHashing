#pragma once
#include "Layout.h"
#include <iostream>

struct Buffer
{
	uint32_t ptr;

	Page page;
};

inline uint32_t addressRoundown(uint32_t address)
{
	return address - address % PAGE_SIZE;
}

inline void zeroPage(void* dst)
{
	memset(dst, 0, PAGE_SIZE);
}

inline uint32_t msBits(uint32_t value, int n)
{
	if (n == 0)
		return 0;

	return value >> (32 - n);
}

inline bool oneBit(uint32_t value, int n)
{
	return msBits(value, n) & 1;
}

inline void showBinary(uint32_t value, int length)
{
	if (length > 1)
		showBinary(value / 2, length - 1);

	std::cout << value % 2;
}
