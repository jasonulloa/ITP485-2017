#include "stdafx.h"
#include "Tree.h"
#include <cstdio>

Tree::Tree() {
	root = nullptr;
}

Tree::~Tree() {
	DeleteNode(this->root);
}

void Tree::DeleteNode(TreeNode *node) {
	if (node != nullptr) {
		DeleteNode(node->left);
		DeleteNode(node->right);
		delete node;
	}
}

void Tree::PushValue(int value) {
	TreeNode *node = new TreeNode();
	node->left = nullptr;
	node->right = nullptr;
	node->value = value;

	//check if empty
	if (this->root == nullptr) {
		this->root = node;
		return;
	}

	TreeNode *curr = this->root;

	//left if less, right if greater or duplicate
	while (curr != nullptr) {
		if (node->value < curr->value) {
			TreeNode *temp = curr->left;
			if (temp == nullptr) {
				curr->left = node;
				return;
			}
			else {
				curr = curr->left;
			}
		}
		else if (node->value >= curr->value) {
			TreeNode *temp = curr->right;
			if (temp == nullptr) {
				curr->right = node;
				return;
			}
			else {
				curr = curr->right;
			}
		}
	}
}

void Tree::PrintValues() {
	std::printf("Sorted result: [");
	PrintNode(this->root);
	std::printf("]");
}

void Tree::PrintNode(TreeNode *node) {
	if (node != nullptr) {
		PrintNode(node->left);
		std::printf("%d, ", node->value);
		PrintNode(node->right);
	}
}