#include "Stack.h"
/*
template <typename T>
Stack<T>::Stack()
{
	item = new T[64];
	maxSize = 64;
	size = 0;
}

template <typename T>
Stack<T>::Stack(int s)
{
	item = new T[s];
	maxSize = s;
	size = s;

	for (int n = 0; n < s; n++)
		item[n] = T(0);
}

template<typename T>
Stack<T>::Stack(const Stack &other, bool deep)
{
	maxSize = other.maxSize;
	size = other.size;
	item = new T[maxSize];
	for (int n = 0; n < size; n++)
		item[n] = other.item[n];
}

template <typename T>
int Stack<T>::add(T newitem)
{

	if (size + 2 > maxSize)
	{
		//printf("adding to stack\n");
		T *temp = new T[maxSize * 2];
		for (int n = 0; n < maxSize; n++)
			temp[n] = item[n];
		delete[] item;

		maxSize *= 2;
		item = temp;
	}

	item[size] = newitem;
	size++;
	return size - 1;
}
*/