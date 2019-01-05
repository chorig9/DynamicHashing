#pragma once
#include "Directory.h"
#include <cstdint>
#include <vector>


template<class T>
bool equals(std::vector<T>&& v1, std::vector<T>&& v2)
{
	if (v1.size() != v2.size())
		return false;

	for (int i = 0; i < v1.size(); i++)
		if (v1[i] != v2[i])
			return false;

	return true;
}

std::vector<uint32_t> showDirectory(Directory& dir)
{
	std::vector<uint32_t> v;

	for (int i = 0; i < (1 << dir.getDepth()); i++)
		v.push_back(dir[i].pagePtr);

	return v;
}