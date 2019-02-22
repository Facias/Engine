#ifndef OCTREE_H_
#define OCTREE_H_

#include "Stack.h"
#include <glm.hpp>

	struct VertexData
	{
		VertexData()
		{
			index = -1;
		}

		int index;
		glm::vec3 pos;
		glm::vec3 norm;
		glm::vec2 uv;
		glm::vec3 color;

	};

	struct IrradianceSample
	{
		IrradianceSample()
		{
		}

		IrradianceSample(glm::vec3 lt, float rad, glm::vec3 npos, glm::vec3 nnorm, glm::vec3 rotGrad[3])
		{
			light = lt;
			radius = rad;
			pos = npos;
			norm = nnorm;
			for (int i = 0; i < 3; i++)
				rotGradient[i] = rotGrad[i];

		}


		glm::vec3 light;
		glm::vec3 pos;
		glm::vec3 norm;
		float radius;
		glm::vec3 rotGradient[3];
		glm::vec3 transGradient[3];
	};

	template <typename T>
	struct OctNode
	{
		OctNode()
		{
			for (int n = 0; n < 8; n++)
				child[n] = NULL;
			//stack = Stack<T>(0);
		}

		char level;
		Stack<T> stack;
		OctNode<T> *child[8];

	};


	template <typename T>
	struct Octree
	{
		Octree();
		Octree(glm::vec3 minb, glm::vec3 maxb, int md);
		inline void expand(OctNode<T> *node, int levels);
		void clean(OctNode<T> *node);


		void add() {};
		//void add(T &dataitem);
		void search() {};
		//int search(T &dataitem);

		glm::vec3 minbound, maxbound;
		int maxdepth;
		OctNode<T> *root;
	};

	template <>
	struct Octree<IrradianceSample>
	{
		Octree();
		Octree(int md);
		Octree(glm::vec3 minb, glm::vec3 maxb, int md);
		void expand(OctNode<IrradianceSample> *node, int levels);

		void add(IrradianceSample &dataitem);
		IrradianceSample *Octree::search(glm::vec3 pos, glm::vec3 norm);
		bool scanRecords(glm::vec3 pos, glm::vec3 norm);



		glm::vec3 minbound, maxbound;
		unsigned char maxdepth;
		OctNode<IrradianceSample> *root;

		float angleMargin = .15f;
	};

	template <>
	struct Octree<VertexData>
	{
		Octree();
		Octree(int md);
		Octree(glm::vec3 minb, glm::vec3 maxb, int md);
		void expand(OctNode<VertexData> *node, int levels);
		void clean(OctNode<VertexData> *node);

		void add(VertexData &dataitem);
		int Octree::search(VertexData &dataitem);

		glm::vec3 minbound, maxbound;
		int maxdepth;
		OctNode<VertexData> *root;


	};

	template <typename T>
	Octree<T>::Octree()
	{
		root = new OctNode<T>();

		minbound = glm::vec3(-10);
		maxbound = glm::vec3(10);
		maxdepth = 4;

	}

	template <typename T>
	Octree<T>::Octree(glm::vec3 minb, glm::vec3 maxb, int md)
	{
		root = new OctNode<T>;
		minbound = minb;
		maxbound = maxb;
		maxdepth = md;
		expand(root, md);
	}

	template <typename T>
	void Octree<T>::expand(OctNode<T> *node, int levels)
	{
		if (levels == 0) return;
		for (int n = 0; n < 8; n++)
		{
			//printf("expand %d %d\n", n, levels);
			node->child[n] = new OctNode<T>();
			expand(node->child[n], levels - 1);
		}
	}



#endif