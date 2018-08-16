#pragma once
#include<iostream>
#include<vector>
using namespace std;


//自定制仿函数，重载operator()，不管是大堆还是小堆，直接传模板参数即可。

template<class T>//大堆
struct Greater
{
	bool operator()(T& h1, T& h2)
	{
		return h1 > h2;
	}
};


template<class T>//小堆
struct Less
{
	bool operator()(T& h1, T& h2)
	{
		return h1 < h2;
	}
};

template<class T, class Compare>
class Heap
{
private:
	vector<T> _hp;
public:
	Heap()//默认的构造函数
	{}

	Heap(T* a, size_t n)//带参数的构造函数
	{
		_hp.reserve(n);
		for(size_t i = 0; i < n; i++)
		{
			_hp.push_back(a[i]);//先将统计出来的次数存放进vector中
		}
		for(int i = (_hp.size()-2)/2; i>=0; i--)//从第一非叶子结点向下调整算法来实现堆
		{
			AdjustDown(i);
		}
	}
	
	void Push(const T& h)//插入
	{
		_hp.push_back(h);//插入后还要调整，因为将其先插在了最后一个元素所在的位置
		AdjustUp(_hp.size()-1);//向上调整
	}
	void Pop()//头删
	{
		assert(!_hp.empty());
		swap(_hp[0],_hp[_hp.size()-1]);//先把头尾做交换
		_hp.pop_back();//将其原来的头删掉
		AdjustDown(0);//向下调整，再次形成堆
	}
	void Print()
	{
		for(size_t i = 0; i<Size(); ++i)
		{
			cout<<_hp[i]<<" ";
		}
		cout<<endl;
	}
	size_t Size()
	{
		return _hp.size();
	}
	T& Top()
	{
		assert(!_hp.empty());
		return _hp[0];
	}
	T& Last()
	{
		assert(!_hp.empty());
		return _hp[_hp.size()-1];
	}
private:
	void AdjustDown(size_t root)
	{
		Compare com;
		size_t parent = root;
		size_t child = parent * 2 + 1;
		while(child < _hp.size())
		{
			if(child + 1 < _hp.size() && com(_hp[child+1],_hp[child]))
			{
				++child;
			}
			if(com(_hp[child],_hp[parent]))
			{
				swap(_hp[child],_hp[parent]);
				parent = child;
				child = parent * 2 + 1;
			}
			else
				break;
		}
	}
	void AdjustUp(size_t root)
	{
		Compare com;
		size_t child = root;
		size_t parent = (child-1)/2;
		while(child > 0)
		{
			if(com(_hp[child], _hp[parent]))
			{
				swap(_hp[child], _hp[parent]);
				child = parent;
				parent = (child-1)/2;
			}
			else
				break;
		}
	}
};

void HeapTest()
{
	int a[] = {10,11,13,12,16,18,15,17,14,19};
	int n = sizeof(a)/sizeof(a[0]);
	Heap<int, Greater<int>> h1(a, n);
	h1.Print();
	Heap<int, Less<int>> h2(a,n);
	h2.Print();
}