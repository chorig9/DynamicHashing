#include <iostream>
#include "DirectoryTest.h"
#include "HashingTest.h"

void help()
{
	std::cout << "help:\n"
		<< "i d k - insert record with data d (sequence of chars) and key k (integer)\n"
		<< "f k - find record with key k\n"
		<< "r k - remove record with key k\n"
		<< "u k d - update record with key k (replace data with d)\n"
		<< "b - show hashing table\n"
		<< "h - help\n"
		<< "k - binary representation of hashed keys (5 ms bits)\n"
		<< "d - default workload\n"
		<< "q - quit\n";
}

void representation(std::function<uintptr_t(int)> hash)
{
	for (int i = 1; i < 32; i++)
	{
		std::cout << i << ": ";
		showBinary(msBits(hash(i), 5), 5);
		std::cout << std::endl;
	}
}

int main()
{
	//DirectoryTest::runTests();
	//HashingTest::runTests();

	char in;

	std::cout << "enter hashing function: 1 - default, 2 - none\n";
	std::cin >> in;

	std::function<uintptr_t(int)> hash;

	if (in == '1')
		hash = FNV1hash;
	else
		hash = [](int key) {return key << 27; };

	Hashing hashing("main", hash, true);
	Record recs[] = { 2,1,4,10,20,16 };;

	help();

	while (1)
	{
		std::cin >> in;

		int key;
		Record r;

		switch (in)
		{
		case 'i':
			std::cin >> r.data;
			std::cin >> r.key;

			std::cout << "hash(key) = ";
			showBinary(msBits(hashing.hash(r.key), 10), 10);
			std::cout << std::endl;

			hashing.insertRecord(r);
			break;
		case 'f':
			std::cin >> key;

			std::cout << "hash(key) = ";
			showBinary(msBits(hashing.hash(r.key), 10), 10);
			std::cout << std::endl;

			r = hashing.getRecord(key);
			if (r.key == NULL)
				std::cout << "not found" << std::endl;
			else
				std::cout << r.data << std::endl;


			break;
		case 'r':
			std::cin >> key;

			std::cout << "hash(key) = ";
			showBinary(msBits(hashing.hash(key), 10), 10);
			std::cout << std::endl;

			hashing.removeRecord(key);
			break;
		case 'u':
			std::cin >> r.key;
			std::cin >> r.data;

			hashing.updateRecord(r);
			break;
		case 'b':
			hashing.show();
			break;
		case 'h':
			help();
			break;
		case 'k':
			representation(hash);
			break;
		case 'd':
			for (int i = 0; i < 6; i++)
			{
				std::cout << "insert " << hash(recs[i].key) << " ";
				showBinary(msBits(hash(recs[i].key), 5), 5);
				std::cout << std::endl;
				hashing.insertRecord(recs[i]);
			}
			break;
		case 'q':
			return 0;
			break;
		}
	}

}
