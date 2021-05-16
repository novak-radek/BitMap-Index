#pragma once

#include <assert.h>
#include <stdlib.h>

class cHeapTable
{
private:
	int mCapacity;
	int mCount;
	char* mData;
	int mRowSize;
	int mColumnCount;

private:
	inline char* GetRowPointer(int rowId);

public:
	cHeapTable(int capacity, int columnCount);
	~cHeapTable();

	bool Add(int* data);
	int Find(int** find, int* queryCounts);

};

cHeapTable::cHeapTable(int capacity, int columnCount)
{
	mCapacity = capacity;
	mCount = 0;
	mRowSize = columnCount * sizeof(int);
	mData = new char[mRowSize * capacity];
	mColumnCount = columnCount;
}

cHeapTable::~cHeapTable()
{
	if (mData != NULL)
	{
		delete mData;
		mData = NULL;
		mCapacity = 0;
		mCount = 0;
	}
}

inline char* cHeapTable::GetRowPointer(int rowId) 
{
	return mData + rowId * mRowSize;
}



bool cHeapTable::Add(int* data)
{
	if (mCount >= mCapacity)
		return false;

	char* p = GetRowPointer(mCount);

	for (int i = 0; i < mColumnCount; i++) {
		*((int*)p) = data[i];
		p += sizeof(int);
	}


	mCount++;

	return true;
}

int cHeapTable::Find(int** data, int* queryCounts)
{
	int res = 0;
	for (int i = 0; i < mCount; i++) {
		char* p = GetRowPointer(i);
		bool rowFound = true;
		for (int j = 0; j < mColumnCount; j++) {
			if (data[j][0] == -1) {
				p += sizeof(int);
				continue;
			}

			bool attrNotFound = true;
			for (int k = 0; k < queryCounts[j]; k++) {
				if (*((int*)p) == data[j][k]) {
					attrNotFound = false;
					break;
				}
			}
			if (attrNotFound) {
				rowFound = false;
				break;
			}
			p += sizeof(int);
		}
		if (rowFound) {
			res++;
		}
	}

	return res;
}

