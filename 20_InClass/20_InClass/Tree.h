#pragma once
#ifndef TREE_H
#define TREE_H

class TreeNode {
public:
	TreeNode *left;
	TreeNode *right;
	int value;
};

class Tree {
public:
	Tree();
	~Tree();
	void DeleteNode(TreeNode *node);
	void PushValue(int value);
	void PrintValues();
	void PrintNode(TreeNode *node);

private:
	TreeNode *root;
};
#endif // !TREE_H