#pragma once

#define _CRT_SECURE_NO_WARNINGS 1
#include"HuffmanTree.h"
#include<string>
#include<stdlib.h>

struct CharInFo
{
	unsigned char _ch;//字符这里用unsigned char，统计出现的字符
	long long _count;//字符出现的次数
	string _code;//Huffman编码

	bool operator < (const CharInFo& info)
	{
		return _count < info._count;
	}
	CharInFo operator + (const CharInFo& info)
	{
		CharInFo ret;
		ret._count = _count + info._count;
		return ret;
	}
	bool operator != (const CharInFo& info)
	{
		return _count != info._count;
	}
	CharInFo(long long count = 0)
		:_ch(0)
		,_count(count)
		,_code("")
	{}
};

class FileCompress
{
	typedef HuffmanTreeNode<CharInFo> Node;
public:
	struct ConfigInfo
	{
		unsigned char _ch;
		long long _count;
	};
	FileCompress()
	{
		//构造函数，初始化数组
		for(int i = 0; i<256; i++)
		{
			_infos[i]._ch = i;//初始化，0~255
			_infos[i]._count = 0;
		}
	}
	bool ComPress(const char* filename)
	{
		assert(filename);
		//1.统计字符出现的次数
		FILE* fout = fopen(filename, "rb");//打开文件以二进制方式读
		assert(fout);
		unsigned char ch = fgetc(fout);//读取文件字符
		long long charcount = 0;
		while(ch != EOF)
		{
			//feof是C语言的标准库函数，是检测流上的文件结束符
			//如果文件结束，则返回非0值，否则返回0
			if(feof(fout))
			{
				break;
			}
			_infos[ch]._count++;//此字符对应的次数
			ch = fgetc(fout);
			charcount++;
		}
		//2.构建Huffman树
		CharInFo invalid;
		invalid._count = 0;//设置非法值的_count为0
		HuffmanTree<CharInFo> tree(_infos, 256, invalid);//构建Huffman树
		Node* root = tree.GetRoot();//Huffman树的根节点

		//3.生成Huffman编码
		string code;//huffman code
		GenerateHuffmanCode(root, code);
		//完成后charinfo中的_code中存的就是HuffmanCode


		//4.压缩
		string compressFile = filename;
		compressFile += ".huffman";//生成压缩文件
		FILE* fin = fopen(compressFile.c_str(), "wb");//以二进制写的形式打开压缩文件
		assert(fin);
		//int fseek ( FILE * stream, long int offset, int origin );
		fseek(fout, 0, SEEK_SET);//把文件指针指向源文件的起始位置
		//向压缩文件里写入Huffman编码
		unsigned char ch1 = fgetc(fout);//读源文件内容
		//这里用 unsigned  因为char是0~255，而中文是负数，就写不进去
		char value = 0;
		int pos = 0;
		while(ch1 != EOF)
		{
			if(feof(fout))
				break;

			string& code = _infos[ch1]._code;//每个字符的Huffman编码
			for(size_t i = 0; i < code.size(); i++)//一个一个比特位来进行放
			{
				value <<= 1;
				//先左移一位，如果code为1就进行下面的代码，把第一位变为1，为0就不处理
				//循环进行，就会把哪一位为1就把那一位置为1
				if(code[i] == '1')//得到二进制的1
				{
					value |= 1;//把原来为0的位置变为1，即就是写进1.
				}
				if(++pos == 8)//满8位写入文件
				{
					fputc(value, fin);//把value写进去，里面存的8个位就是HuffmanCode
					value = 0;//重置，循环写
					pos = 0;
				}
			}
			ch1 = fgetc(fout);
		}
		//到这里文件就读完了
		if(pos)//最后一个编码不足8个位
		{
			value = value << (8-pos);//不足一个字节，给后面补0再写进fin文件
			fputc(value, fin);
		}

		//5.写配置信息，字符和字符出现的次数都写进去
		string ConfigFilename = filename;
		ConfigFilename += ".config";
		FILE* fConfig = fopen(ConfigFilename.c_str(),"wb");
		assert(fConfig);
		char countStr[32];//字符出现的次数存放的数组

		//char *  itoa ( int value, char * str, int base );
		itoa(charcount, countStr, 10);//把统计的次数转换成10进制
		fputs(countStr, fConfig);//把总次数写入
		fputc('\n', fConfig);//换行
		for(int i = 0; i < 256; i++)
		{
			string put;
			if(_infos[i] != invalid)
			{
				fputc(_infos[i]._ch, fConfig);
				//必须先把ch放进去，如果把ch作为string的字符最后转换为c的字符，会导致'\0'没有处理
				put.push_back(',');
				itoa(_infos[i]._count, countStr, 10);

				put += countStr;
				fputs(put.c_str(), fConfig);
				fputc('\n', fConfig);
			}
		}
		fclose(fin);
		fclose(fout);//关闭掉刚才打开的文件
		fclose(fConfig);
		return true;
	}
	bool UnCompress(const char* filename)
	{                                             
		//1.读取配置文件   
		string ConfigFilename = filename;  
		int count = ConfigFilename.rfind('.');  
		ConfigFilename = ConfigFilename.substr(0, count);  
		string UnCompressname = ConfigFilename + ".unpress";   

		FILE* fUnCompress = fopen(UnCompressname.c_str(), "wb"); //创建解压缩文件  
		ConfigFilename += ".config"; 

		FILE* fconfig = fopen(ConfigFilename.c_str(),"rb");
		assert(fconfig);
		assert(fUnCompress);

		FILE* fpress = fopen(filename,"rb");//打开压缩好的文件
		assert(fpress);

		long long charCount = 0;//找出字符出现的次数

		string line;
		_ReadLine(fconfig,line);

		charCount = atoi(line.c_str());//第一行存放字符出现的总数
		line.clear();

		while(_ReadLine(fconfig,line))//文件结束会返回0
		{
			if(!line.empty())
			{
				unsigned char ch = line[0];//先读出字符
				string tmp = line.substr(2);//把前两个字符剪切掉，剩下的是字符出现的次数
				//字符串的剪切，读出次数，在文件中都是以字符串的形式存在
				_infos[(unsigned char)ch]._count = atoi(tmp.c_str());  
				line.clear();
			}
			else//空行
			{
				line += '\n';
			}
		}
		//再次构建huffma树
		CharInFo invalid;
		invalid._count = 0;
		HuffmanTree<CharInFo> tree(_infos,256,invalid);
		Node* root = tree.GetRoot();
		Node* cur = root;
		char ch = fgetc(fpress);//读压缩文件
		int pos = 8;
		while(1)
		{
			--pos;
			if((ch>>pos) & 1)//为1往右走
			{
				cur = cur->_right;
			}
			else//为0往左走
			{
				cur = cur->_left;
			}
			if(cur->_left==NULL && cur->_right==NULL)//左右都为空，叶子，读出来
			{
				fputc(cur->_w._ch,fUnCompress);//读到解压缩的文件中
				cur = root;//再次从根节点遍历
				charCount--;//字符总数减减
			}
			if(pos == 0)//一个字节读完，重置，读下一个字节
			{
				ch = fgetc(fpress);
				pos = 8;//重置
			}
			if(charCount == 0)//当字符总数为0,则停止读
			{
				break;
			}
		}
		fclose(fconfig);
		fclose(fUnCompress);
		fclose(fpress);
		return true;
	}
protected:
	bool _ReadLine(FILE* filename,string& line)
	{
		assert(filename);
		if(feof(filename))
		{
			return false;
		}
		unsigned char ch = fgetc(filename);

		while(ch != '\n')
		{
			line += ch;
			ch = fgetc(filename);
			if(feof(filename))
			{
				return false;
			}
		}
		return true;
	}
	void GenerateHuffmanCode(Node* cur, string code)//获取Huffman编码函数
	{
		if(cur == NULL)
			return; 
		if(cur->_left == NULL && cur->_right == NULL)
		{
			_infos[(unsigned char)cur->_w._ch]._code = code;
			return;
		}
		GenerateHuffmanCode(cur->_left, code + '0');//向左为0
		GenerateHuffmanCode(cur->_right, code + '1');//向右为1
	}
protected:
	CharInFo _infos[256];
};

void TestCompress()
{
	FileCompress hft;
	int begin = GetTickCount();
	hft.ComPress("2.doc");
	int end = GetTickCount();
	cout<< end-begin << endl;
}

void TestUnCompress()  
{  
	FileCompress hf;  
	int begin = GetTickCount();  
	hf.UnCompress("2.doc.huffman");  
	int end = GetTickCount();  
	cout << end - begin << endl;  
}  
