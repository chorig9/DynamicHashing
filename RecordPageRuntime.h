#pragma once
#include <functional>
#include "Storage.h"

class RecordPageRuntime
{
public:
	RecordPageRuntime(uint32_t address);

	RecordPage* operator->();

	Page& operator*();

	bool isFull();

	bool isEmpty();

	int nRecords();

	uint32_t getAddress();

	bool insertRecord(Record record);

	Record getRecord(int key);

	bool removeRecord(int key);

	bool updateRecord(Record record);

	void transferRecordsToNewPage(RecordPageRuntime& page,
		std::function<bool(Record&)> tranferRecordDecision);

private:

	Page page;

	uint32_t address;
};