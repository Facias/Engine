#include "BinaryTree.h"



bool BinaryTree<float>::find(float val)
{
	current = root;

	while (current != NULL)
	{
		if (current->value == val)
			return true;
		else
		{
			if (current->value > val)
			{
				current = current->left;
			}

			if (current->value < val)
			{
				current = current->right;
			}
		}
	}

	return false;
}

bool BinaryTree<int>::remove(int val)
{
	current = root;

	while (current != NULL)
	{
		if (current->value == val)
		{
			// if found, remove or replace
			// if node has no children, delete this node completely
			if (current->left == NULL && current->right == NULL)
			{
				delete current;
				current = NULL;
				return true;
			}
			// if current only has right child
			if (current->left == NULL && current->right != NULL)
			{
				// if current right of parent
				if (current->parent->value < current->value)
				{
					current->parent->right = current->right;
					delete current;
					current = NULL;
					return true;
				}
				// if current left of parent
				if (current->parent->value > current->value)
				{
					current->parent->left = current->right;
					delete current;
					current = NULL;
					return true;
				}
			}
			// if current only has left child
			if (current->left == NULL && current->right != NULL)
			{
				// if current right of parent
				if (current->parent->value < current->value)
				{
					current->parent->right = current->left;
					delete current;
					current = NULL;
					return true;
				}
				// if current left of parent
				if (current->parent->value > current->value)
				{
					current->parent->left = current->left;
					delete current;
					current = NULL;
					return true;
				}
			}
			// if current has both children
			if (current->left != NULL && current->right != NULL)
			{
				// if current right of parent
				if (current->parent->value < current->value)
				{
					// find furthest right item of left child
					TreeNode<int> *seeker = current->left;
					while (seeker->right != NULL)
					{
						seeker = seeker->right;
					}
					current->value = seeker->value;
					if (seeker->left != NULL)
					{
						seeker->parent->right = seeker->left;
					}
					delete seeker;
					return true;
				}
				// if current left of parent
				if (current->parent->value > current->value)
				{
					// find furthest left item of right child
					TreeNode<int> *seeker = current->right;
					while (seeker->left != NULL)
					{
						seeker = seeker->left;
					}
					current->value = seeker->value;
					if (seeker->right != NULL)
					{
						seeker->parent->left = seeker->right;
					}
					delete seeker;
					return true;
				}
			}

		}
		else
		{
			if (current->value > val)
			{
				current == current->left;
			}

			if (current->value < val)
			{
				current == current->right;
			}
		}
	}

	return false;
}

void BinaryTree<int>::balance()
{
	// find median values - not mean values
	// count all left children and all right children from root
	// the median node shall become the new root

	short int countLeft = 0;
	current = root;
	while (current->left != NULL)
	{
		current = current->left;
		countLeft++;
	}
	
	short int countRight = 0;
	current = root;
	while (current->right != NULL)
	{
		current = current->right;
		countRight++;
	}

	//take difference
	int diff = abs(countLeft - countRight);
	if (diff < 2) return;

	if (countLeft > countRight)
	{
		int move = 0;
		TreeNode<int> *newRoot;
		current = root;

		while (move < diff / 2)
		{
			// search left child's rightmost child,
			// and append this root node to that as right child
			TreeNode<int> *seeker = current->left;
			while (seeker->right != NULL)
			{
				seeker = seeker->right;
			}
			seeker->right = current;
			current->parent = seeker->right;
			
			newRoot = current->left;
			current->left = NULL;

			current = newRoot;
		}
		return;
	}

	if (countLeft < countRight)
	{
		int move = 0;
		TreeNode<int> *newRoot;
		current = root;

		while (move < diff / 2)
		{
			// search left child's rightmost child,
			// and append this root node to that as right child
			TreeNode<int> *seeker = current->right;
			while (seeker->left != NULL)
			{
				seeker = seeker->left;
			}
			seeker->left = current;
			current->parent = seeker->left;

			newRoot = current->right;
			current->right = NULL;

			current = newRoot;

		}
		return;
	}
}