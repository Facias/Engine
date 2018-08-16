#ifndef STACK_H_
#define STACK_H_

template <typename T>
struct Stack
{
	Stack()
	{
		item = new T[64];
		maxSize = 64;
		size = 0;
	}


	Stack(int s)
	{
		item = new T[s];
		maxSize = s;
		size = s;

		for (int n = 0; n < s; n++)
			item[n] = T(0);
	}

	Stack(const Stack &other, bool deep)
	{
		maxSize = other.maxSize;
		size = other.size;
		item = new T[maxSize];
		for (int n = 0; n < size; n++)
			item[n] = other.item[n];
	}



	T *item;
	unsigned int maxSize;
	unsigned int size;

	void clean()
	{
		delete[] item;
		maxSize = 0;
		size = 0;
	}


	// WRONG
	Stack add(Stack &other)
	{
		Stack temp;
		temp.size = size + other.size;
		temp.maxSize = maxSize + other.maxSize;
		temp.item = new T[temp.maxSize];
		for (int n = 0; n < maxSize; n++)
			temp.item[n] = item[n];
		for (int n = 0; n < other.size; n++)
			temp.item[n] = other.item[n];
	}

	static Stack add(Stack &stack0, Stack &stack1)
	{
		Stack temp;
		temp.size = stack0.size + stack1.size;
		temp.maxSize = stack0.maxSize + stack1.maxSize;
		temp.item = new T[temp.maxSize];
		for (int n = 0; n < stack0.size; n++)
			temp.item[n] = stack0.item[n];
		for (int n = 0; n < stack1.size; n++)
			temp.item[n+stack0.size] = stack1.item[n];
		

		return temp;
	}

	int add(T newitem)
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
};



#endif
