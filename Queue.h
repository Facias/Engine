#ifndef QUEUE_H_
#define QUEUE_H_


template <typename T>
struct Queue
{
	Queue()
	{
		item = new T[16];
		maxSize = 16;
		size = 0;

	}


	Queue(int s)
	{
		item = new T[s];
		maxSize = s;
		size = s;

	}

	Queue(const Queue &other, bool deep)
	{
		maxSize = other.maxSize;
		size = other.size;
		item = new T[maxSize];
		for (uint n = 0; n < size; n++)
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

	static Queue add(Queue &queue0, Queue &queue1)
	{
		Queue temp;
		temp.size = queue0.size + queue1.size;
		temp.maxSize = queue0.maxSize + queue1.maxSize;
		temp.item = new T[temp.maxSize];
		for (uint n = 0; n < queue0.size; n++)
			temp.item[n] = queue0.item[n];
		for (uint n = 0; n < queue1.size; n++)
			temp.item[n + queue0.size] = queue1.item[n];


		return temp;
	}

	int add(T newitem)
	{

		if (size + 2 > maxSize)
		{
			//printf("adding to stack\n");
			T *temp = new T[maxSize * 2];
			for (uint n = 0; n < maxSize; n++)
				temp[n] = item[n];
			delete[] item;

			maxSize *= 2;
			item = temp;
		}

		item[size] = newitem;
		size++;
		return size - 1;
	}

	T pop()
	{
		T remove = item[0];
		for (int n = 0; n < size - 1; n++)
		{
			item[n] = item[n + 1];
		}
		size--;
		return remove;

	}


};

#endif
