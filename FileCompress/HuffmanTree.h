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

	W _w;//Ȩֵ��Ҳ���ǹ���������Ҷ�ӽڵ������

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
	//����Huffman��
	HuffmanTree(W* a, size_t n, const W& invalid)
	{
		struct NodeCompare//�Զ���ȽϷ�ʽ��ͨ�����ǵ�Ȩֵ���Ƚ�
		{
			bool operator()(Node* r, Node* l)
			{
				return r->_w < l->_w;
			}
		};
		assert(a);
		Heap<Node*, NodeCompare> minHeap;//С��
		for(size_t i = 0; i<n; i++)
		{
			//�õ��˽ṹ��Ĵ�С�Ƚϣ���Ҫ�Լ�����ȽϷ�ʽ
			if(a[i] != invalid)//��Ϊ�Ƿ�ֵ����push��������
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
			//�õ��˽ṹ����ӣ���Ҫ�Լ�����ӵķ�ʽ
			Node* parent = new Node(left->_w + right->_w);
			minHeap.Push(parent);

			parent->_left = left;
			parent->_right = right;
			left->_parent = parent;
			right->_parent = parent;
		}
		_root = minHeap.Top();//���Ѹ�����ȥ
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