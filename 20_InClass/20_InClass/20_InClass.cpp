// 20_InClass.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "Tree.h"

int main()
{
	int data[] = {
		60, 36, 25, 44, 62, 58, 25, 66, 84, 48,
		1, 63, 6, 58, 40, 73, 30, 100, 88, 60,
		100, 73, 77, 55, 6, 33, 69, 4, 93, 33,
		36, 53, 79, 2, 35, 41, 64, 76, 70, 81,
		55, 22, 20, 44, 93, 58, 14, 6, 60, 36
	};

	Tree *binaryTree = new Tree();
	int size = sizeof(data) / sizeof(int);

	for (int i = 0; i < size; i++) {
		binaryTree->PushValue(data[i]);
	}

	binaryTree->PrintValues();

    return 0;
}