#include "RecordPageRuntime.h"
#include "utils.h"

RecordPage* RecordPageRuntime::operator->()
{
	return &page.data.records;
}

RecordPageRuntime::RecordPageRuntime(uint32_t address) : address(address)
{
}

bool RecordPageRuntime::isFull()
{
	for (int i = 0; i < RECORDS_PER_PAGE; i++)
		if (page.data.records.record[i].isNull())
			return false;

	return true;
}

bool RecordPageRuntime::isEmpty()
{
	for (int i = 0; i < RECORDS_PER_PAGE; i++)
		if (!page.data.records.record[i].isNull())
			return false;

	return true;
}

int RecordPageRuntime::nRecords()
{
	int n = 0;

	for (int i = 0; i < RECORDS_PER_PAGE; i++)
		if (!page.data.records.record[i].isNull())
			n++;

	return n;
}

uint32_t RecordPageRuntime::getAddress()
{
	return address;
}

bool RecordPageRuntime::insertRecord(Record record)
{
	int i;
	for (i = 0; i < RECORDS_PER_PAGE; i++)
		if (page.data.records.record[i].isNull())
			break;

	if (i < RECORDS_PER_PAGE)
	{
		page.data.records.record[i] = record;
		return true;
	}

	return false;
}

Record RecordPageRuntime::getRecord(int key)
{
	for (int i = 0; i < RECORDS_PER_PAGE; i++)
		if (page.data.records.record[i].key == key)
			return page.data.records.record[i];

	return Record::null();
}

bool RecordPageRuntime::removeRecord(int key)
{
	for (int i = 0; i < RECORDS_PER_PAGE; i++)
		if (page.data.records.record[i].key == key)
		{
			page.data.records.record[i] = Record::null();
			return true;
		}

	return false;
}

bool RecordPageRuntime::updateRecord(Record record)
{
	for (int i = 0; i < RECORDS_PER_PAGE; i++)
		if (page.data.records.record[i].key == record.key)
		{
			page.data.records.record[i] = record;
			return true;
		}

	return false;
}

void RecordPageRuntime::transferRecordsToNewPage(RecordPageRuntime& newPage,
	std::function<bool(Record&)> tranferRecordDecision)
{
	zeroPage(&newPage);

	for (int i = 0; i < RECORDS_PER_PAGE; i++)
	{
		if (tranferRecordDecision(page.data.records.record[i]))
			std::swap(newPage->record[i], page.data.records.record[i]);
	}
}

Page& RecordPageRuntime::operator*()
{
	return page;
}