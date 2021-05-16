#include <stdio.h>
#include <chrono>
#include <iostream>

#include "cHeapTable.h"
#include "cBitMapIndex.h"

using namespace std;
using namespace std::chrono;

int main()
{
	int const RowCount = 10000000;
	int const ColumnCount = 10;
	int const QueryCount = 1000;

	int cardinality[ColumnCount] = { 0 };
	int numOfBits = 0;

	for (int i = 0; i < ColumnCount; i++) {
		int cardinalityForColumn = rand() % 6 + 5;
		cardinality[i] = cardinalityForColumn;
		numOfBits += cardinalityForColumn;
	}

	cBitMapIndex* bi = new cBitMapIndex(RowCount, ColumnCount, cardinality, numOfBits);
	cHeapTable* ht = new cHeapTable(RowCount, ColumnCount);

	auto tmp1 = std::chrono::high_resolution_clock::now();
	auto tmp2 = std::chrono::high_resolution_clock::now();
	auto bitMapAdd = std::chrono::duration_cast<std::chrono::duration<double>>(tmp1 - tmp2);
	auto heapAdd = std::chrono::duration_cast<std::chrono::duration<double>>(tmp1 - tmp2);
	auto bitMapFind = std::chrono::duration_cast<std::chrono::duration<double>>(tmp1 - tmp2);
	auto heapFind = std::chrono::duration_cast<std::chrono::duration<double>>(tmp1 - tmp2);

	
	for (int i = 0; i < RowCount; i++) {
		int* data = new int[ColumnCount];

		for (int j = 0; j < ColumnCount; j++) {

			data[j] = rand() % cardinality[j];
		}


		auto t1 = std::chrono::high_resolution_clock::now();
		bi->Add(data);
		auto t2 = std::chrono::high_resolution_clock::now();
		bitMapAdd += std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);


		t1 = std::chrono::high_resolution_clock::now();
		ht->Add(data);
		t2 = std::chrono::high_resolution_clock::now();
		heapAdd += std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);

		delete[] data;
	}

	

	int queryCounts[ColumnCount] = { 0 };

	for (int i = 0; i < ColumnCount; i++) {
		queryCounts[i] = rand() % 2 + 1;
	}

	for (int i = 0; i < QueryCount; i++) {

		int** query = new int*[ColumnCount];
		for (int j = 0; j < ColumnCount; j++) {
			query[j] = new int[queryCounts[j]];
		}

		for (int j = 0; j < ColumnCount; j++) {
			if (rand() % 2) {
				query[j][0] = -1;
			}
			else {
				for (int k = 0; k < queryCounts[j]; k++) {
					query[j][k] = rand() % cardinality[j];
				}
			}
		}

		auto t1 = std::chrono::high_resolution_clock::now();
		int biResult = bi->Find(query, queryCounts);
		auto t2 = std::chrono::high_resolution_clock::now();
		bitMapFind += std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);

		t1 = std::chrono::high_resolution_clock::now();
		int htResult = ht->Find(query, queryCounts);
		t2 = std::chrono::high_resolution_clock::now();
		heapFind += std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);;

		if (i % 100 == 0) {
			printf("BitMap index found %d rows.\n", biResult);
			printf("Heaptable found %d rows.\n-----------------------------------------------\n", htResult);
		}

		for (int j = 0; j < ColumnCount; j++) {
			delete[] query[j];
		}
		delete[] query;
	}

	printf("\nBitMap Index add total duration: %.5fs\n", bitMapAdd);
	printf("Heap table add total duration: %.5fs\n", heapAdd);
	printf("BitMap Index find total duration: %.5fs\n", bitMapFind);
	printf("Heap table find total duration: %.5fs\n", heapFind);


	delete bi;
	delete ht;

	return 0;
}

