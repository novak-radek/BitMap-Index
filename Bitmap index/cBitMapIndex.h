#pragma once

#include <stdlib.h>
#include <iostream>

class cBitMapIndex 
{
private:
	int mCapacity;
	int mCount;
	unsigned char* mData;
	int mRowSize;
	int mColumnCount;
	int mNumOfBits;
	int* mCardinality;

private:
	inline unsigned char* GetRowPointer(int rowId) const;

public:
	cBitMapIndex(int capacity, int columnCount, int* cardinality, int numOfBits);
	~cBitMapIndex();

	bool Add(int* data);
	int Find(int** find, int* queryCounts);
};

cBitMapIndex::cBitMapIndex(int capacity, int columnCount, int* cardinality, int numOfBits)
{
	mCapacity = capacity;
	mCount = 0;
	mColumnCount = columnCount;
	mCardinality = cardinality;
	mNumOfBits = numOfBits;

	mRowSize = ceil((double)numOfBits / 8);
	mData = new unsigned char[mRowSize * capacity];

	printf("Creating BitMap Index with %d rows and row size %d.\nSize of columns: ", mCapacity, mRowSize);
	for (int i = 0; i < mColumnCount; i++) {
		if (i == mColumnCount-1)
			printf("%d bits.", mCardinality[i]);
		else
			printf("%d, ", mCardinality[i]);
	}
	printf("\nTotal bits: %d\n\n", mNumOfBits);
}

cBitMapIndex::~cBitMapIndex()
{
	if (mData != NULL)
	{
		delete[] mData;
		mCapacity = 0;
		mCount = 0;
	}
	mCardinality = NULL;
}

inline unsigned char* cBitMapIndex::GetRowPointer(int rowId) const
{
	return mData + rowId * mRowSize;
}

bool cBitMapIndex::Add(int* data)
{
	if (mCount >= mCapacity)
		return false;

	unsigned char* p = GetRowPointer(mCount);

	int index = 0;
	int missingBits = 0;

	for (int i = 0; i < mRowSize; i++) {
		int count = 0;
		bool firstRun = true;

		while (count < 8) {
			if (index == mColumnCount)
				break;

			if (missingBits < 0)
				count -= missingBits;
			else
				count += mCardinality[index];

			if (firstRun) {
				if (missingBits < 0)
					missingBits = 8 + missingBits;
				else
					missingBits = 8 - (mCardinality[index] - missingBits);

				int shift = data[index] + missingBits;
				*((unsigned char*)p) = (1 << shift) & 0xFF;
				firstRun = false;
				if (missingBits >= 0)
					index++;
			}
			else {
				if (mCardinality[index] <= missingBits) {
					missingBits = missingBits - (mCardinality[index] % 8);
					int shift = data[index] + missingBits;
					*((unsigned char*)p) |= (1 << shift) & 0xFF;
					index++;
				}
				else {
					int  shift = data[index] - (mCardinality[index] - missingBits);
					missingBits -= mCardinality[index];
					*((unsigned char*)p) |= (1 << shift) & 0xFF;
				}
			}
		}
		p += sizeof(unsigned char);
	}

	mCount++;


	return true;
}


int cBitMapIndex::Find(int** data, int* queryCounts)
{

	int index = 0;
	int missingBits = 0;
	int  shift = 0;
	unsigned char* query = new unsigned char[mRowSize];

	for (int i = 0; i < mRowSize; i++) {
		int count = 0;
		bool firstRun = true;

		while (count < 8) {
			if (index == mColumnCount)
				break;

			if (missingBits < 0)
				count -= missingBits;
			else
				count += mCardinality[index];

			if (firstRun) {
				if (missingBits < 0)
					missingBits = 8 + missingBits;
				else
					missingBits = 8 - (mCardinality[index] - missingBits);

				if (data[index][0] == -1) {
					shift = missingBits;
					query[i] = (((1 << mCardinality[index])-1) << (shift < 0 ? 0 : shift));
				}
				else {
					for (int j = 0; j < queryCounts[index]; j++) {
						shift = data[index][j] + missingBits;

						if (j == 0)
							query[i] = (1 << shift);
						else
							query[i] |= (1 << shift);
					}
				}
				
				firstRun = false;
				if (missingBits >= 0)
					index++;
			}
			else {
				if (mCardinality[index] <= missingBits) {
					missingBits = missingBits - (mCardinality[index] % 8);
					if (data[index][0] == -1) {
						shift = missingBits;
						query[i] |= (((1 << (mCardinality[index] + shift)) - 1) ^ ((1 << missingBits) - 1));
					}
					else {
						for (int j = 0; j < queryCounts[index]; j++) {
							shift = data[index][j] + missingBits;
							query[i] |= (1 << shift);
						}
					}
					index++;
				}
				else {
					if (data[index][0] == -1) {
						shift = missingBits;
						query[i] |= ((1 << shift) - 1);
					}
					else {
						for (int j = 0; j < queryCounts[index]; j++) {
							shift = data[index][j] - (mCardinality[index] - missingBits);
							query[i] |= (1 << shift);
						}
					}
					missingBits -= mCardinality[index];
				}
			}
		}
	}

	int res = 0;

	for (int i = 0; i < mCount; i++) {
		unsigned char* p = GetRowPointer(i);

		bool rowFound = true;

		for (int j = 0; j < mRowSize; j++) {

			if ((*((unsigned char*)p) & query[j]) != *((unsigned char*)p)) {
				//std::cout << (int)*p << ' ' << (int)query[j] << '\n';
				rowFound = false;
				break;
			}
			p += sizeof(unsigned char);
		}
		if (rowFound){
			//for (int j = 0; j < mRowSize; j++) {

			//	std::cout << (int)query[j] << '\t';

			//}
			//std::cout << '\n';
			//unsigned char* p = GetRowPointer(i);
			//for (int j = 0; j < mRowSize; j++) {

			//	std::cout << (int)*((unsigned char*)p) << '\t';
			//	
			//	p += sizeof(unsigned char);
			//}
			//std::cout << '\n';
			//std::cout << '\n';
			res++;
		}
	}

	delete[] query;

	return res;
}