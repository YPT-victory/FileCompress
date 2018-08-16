#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
#include<iostream>
#include<assert.h>
#include<Windows.h>
#include<stdio.h>
#include"Heap.h"
using namespace std;

template<class W>
struct HuffmanTreeNode
{
	HuffmanTreeNode<W>* _left;
	HuffmanTreeNode<W>* _right;
	HuffmanTreeNode<W>* _parent;

	W _w;//权值，也就是哈夫曼树的叶子节点的数据

	HuffmanTreeNode(const W& w)
		:_left(NULL)
		,_right(NULL)
		,_parent(NULL)
		,_w(w)
	{}
};

template<class W>
class HuffmanTree
{
	typedef HuffmanTreeNode<W> Node;
public:
	HuffmanTree()
		:_root(NULL)
	{}
	//构建Huffman树
	HuffmanTree(W* a, size_t n, const W& invalid)
	{
		struct NodeCompare//自定义比较方式，通过它们的权值来比较
		{
			bool operator()(Node* r, Node* l)
			{
				return r->_w < l->_w;
			}
		};
		assert(a);
		Heap<Node*, NodeCompare> minHeap;//小堆
		for(size_t i = 0; i<n; i++)
		{
			//用到了结构体的大小比较，需要自己定义比较方式
			if(a[i] != invalid)//不为非法值，就push到堆里面
			{
				minHeap.Push(new Node(a[i]));
			}
		}
		while(minHeap.Size() > 1)
		{
			Node* left = minHeap.Top();
			minHeap.Pop();
			Node* right = minHeap.Top();
			minHeap.Pop();
			//用到了结构体相加，需要自己定义加的方式
			Node* parent = new Node(left->_w + right->_w);
			minHeap.Push(parent);

			parent->_left = left;
			parent->_right = right;
			left->_parent = parent;
			right->_parent = parent;
		}
		_root = minHeap.Top();//最后把根给过去
	}
	Node* GetRoot()
	{
		return _root;
	}
	~HuffmanTree()
	{
		Destroy(_root);
		_root = NULL;
	}
	void Print()
	{
		_Print(_root);
		cout<<endl;
	}
private:
	void _Print(Node* root)
	{
		if(root != NULL)
		{
			cout<<root->_w<<" ";
			_Print(root->_left);
			_Print(root->_right);
		}
	}
	void Destroy(Node* root)
	{
		if(root != NULL)
		{
			Destroy(root->_left);
			Destroy(root->_right);
			delete root;
		}
	}
private:
	Node* _root;
};

void HuffmanTreeTest()
{
	int a[] = {1,2,3,4,5};
	HuffmanTree<int> ht(a, sizeof(a)/sizeof(a[0]), '#');
	ht.Print();
}