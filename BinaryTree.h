#ifndef NULL
	#define NULL nullptr
#endif

#ifndef BINARY_TREE_H_
#define BINARY_TREE_H_

#include "glm.hpp"

template <typename T>
struct TreeNode
{
	TreeNode()
	{
		left = NULL;
		right = NULL;
		parent = NULL;
	}

	T value;
	TreeNode<T> *left;
	TreeNode<T> *right;
	TreeNode<T> *parent;
};

template <typename T>
struct BinaryTree
{
	BinaryTree()
	{
		root = TreeNode();
		current = root;
	}

	bool find(float val);
	bool remove(int val);

	// simple balance of right and left children
	// do this for each node for a full tree balance
	void balance();

	TreeNode<T> *root;
	TreeNode<T> *current;
};

#endif


