#ifndef HASH_TABLE_H_
#define HAST_TABLE_H_

template <typename T>
struct HashNode
{
	T item;
	HashNode *next;
};

template <typename T>
struct HashTable
{

	HashTable()
	{
		table = new HashNode[100];
		itemCount = 0;
		tableSize = 100;
	}

	HashTable(int size)
	{
		table = new T[size];
		itemCount = 0;
		tableSize = size;
	}

	Hash(int val)
	{

	}

	find(int val)
	{

	}

	HashNode *table;
	unsigned short int itemCount;
	unsigned short int tableSize;
};


#endif

