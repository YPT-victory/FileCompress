#pragma once
#include<iostream>
#include<vector>
using namespace std;


//�Զ��Ʒº���������operator()�������Ǵ�ѻ���С�ѣ�ֱ�Ӵ�ģ��������ɡ�

template<class T>//���
struct Greater
{
	bool operator()(T& h1, T& h2)
	{
		return h1 > h2;
	}
};


template<class T>//С��
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
	Heap()//Ĭ�ϵĹ��캯��
	{}

	Heap(T* a, size_t n)//�������Ĺ��캯��
	{
		_hp.reserve(n);
		for(size_t i = 0; i < n; i++)
		{
			_hp.push_back(a[i]);//�Ƚ�ͳ�Ƴ����Ĵ�����Ž�vector��
		}
		for(int i = (_hp.size()-2)/2; i>=0; i--)//�ӵ�һ��Ҷ�ӽ�����µ����㷨��ʵ�ֶ�
		{
			AdjustDown(i);
		}
	}
	
	void Push(const T& h)//����
	{
		_hp.push_back(h);//�����Ҫ��������Ϊ�����Ȳ��������һ��Ԫ�����ڵ�λ��
		AdjustUp(_hp.size()-1);//���ϵ���
	}
	void Pop()//ͷɾ
	{
		assert(!_hp.empty());
		swap(_hp[0],_hp[_hp.size()-1]);//�Ȱ�ͷβ������
		_hp.pop_back();//����ԭ����ͷɾ��
		AdjustDown(0);//���µ������ٴ��γɶ�
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