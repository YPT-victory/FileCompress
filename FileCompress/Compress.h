#pragma once

#define _CRT_SECURE_NO_WARNINGS 1
#include"HuffmanTree.h"
#include<string>
#include<stdlib.h>

struct CharInFo
{
	unsigned char _ch;//�ַ�������unsigned char��ͳ�Ƴ��ֵ��ַ�
	long long _count;//�ַ����ֵĴ���
	string _code;//Huffman����

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
		//���캯������ʼ������
		for(int i = 0; i<256; i++)
		{
			_infos[i]._ch = i;//��ʼ����0~255
			_infos[i]._count = 0;
		}
	}
	bool ComPress(const char* filename)
	{
		assert(filename);
		//1.ͳ���ַ����ֵĴ���
		FILE* fout = fopen(filename, "rb");//���ļ��Զ����Ʒ�ʽ��
		assert(fout);
		unsigned char ch = fgetc(fout);//��ȡ�ļ��ַ�
		long long charcount = 0;
		while(ch != EOF)
		{
			//feof��C���Եı�׼�⺯�����Ǽ�����ϵ��ļ�������
			//����ļ��������򷵻ط�0ֵ�����򷵻�0
			if(feof(fout))
			{
				break;
			}
			_infos[ch]._count++;//���ַ���Ӧ�Ĵ���
			ch = fgetc(fout);
			charcount++;
		}
		//2.����Huffman��
		CharInFo invalid;
		invalid._count = 0;//���÷Ƿ�ֵ��_countΪ0
		HuffmanTree<CharInFo> tree(_infos, 256, invalid);//����Huffman��
		Node* root = tree.GetRoot();//Huffman���ĸ��ڵ�

		//3.����Huffman����
		string code;//huffman code
		GenerateHuffmanCode(root, code);
		//��ɺ�charinfo�е�_code�д�ľ���HuffmanCode


		//4.ѹ��
		string compressFile = filename;
		compressFile += ".huffman";//����ѹ���ļ�
		FILE* fin = fopen(compressFile.c_str(), "wb");//�Զ�����д����ʽ��ѹ���ļ�
		assert(fin);
		//int fseek ( FILE * stream, long int offset, int origin );
		fseek(fout, 0, SEEK_SET);//���ļ�ָ��ָ��Դ�ļ�����ʼλ��
		//��ѹ���ļ���д��Huffman����
		unsigned char ch1 = fgetc(fout);//��Դ�ļ�����
		//������ unsigned  ��Ϊchar��0~255���������Ǹ�������д����ȥ
		char value = 0;
		int pos = 0;
		while(ch1 != EOF)
		{
			if(feof(fout))
				break;

			string& code = _infos[ch1]._code;//ÿ���ַ���Huffman����
			for(size_t i = 0; i < code.size(); i++)//һ��һ������λ�����з�
			{
				value <<= 1;
				//������һλ�����codeΪ1�ͽ�������Ĵ��룬�ѵ�һλ��Ϊ1��Ϊ0�Ͳ�����
				//ѭ�����У��ͻ����һλΪ1�Ͱ���һλ��Ϊ1
				if(code[i] == '1')//�õ������Ƶ�1
				{
					value |= 1;//��ԭ��Ϊ0��λ�ñ�Ϊ1��������д��1.
				}
				if(++pos == 8)//��8λд���ļ�
				{
					fputc(value, fin);//��valueд��ȥ��������8��λ����HuffmanCode
					value = 0;//���ã�ѭ��д
					pos = 0;
				}
			}
			ch1 = fgetc(fout);
		}
		//�������ļ��Ͷ�����
		if(pos)//���һ�����벻��8��λ
		{
			value = value << (8-pos);//����һ���ֽڣ������油0��д��fin�ļ�
			fputc(value, fin);
		}

		//5.д������Ϣ���ַ����ַ����ֵĴ�����д��ȥ
		string ConfigFilename = filename;
		ConfigFilename += ".config";
		FILE* fConfig = fopen(ConfigFilename.c_str(),"wb");
		assert(fConfig);
		char countStr[32];//�ַ����ֵĴ�����ŵ�����

		//char *  itoa ( int value, char * str, int base );
		itoa(charcount, countStr, 10);//��ͳ�ƵĴ���ת����10����
		fputs(countStr, fConfig);//���ܴ���д��
		fputc('\n', fConfig);//����
		for(int i = 0; i < 256; i++)
		{
			string put;
			if(_infos[i] != invalid)
			{
				fputc(_infos[i]._ch, fConfig);
				//�����Ȱ�ch�Ž�ȥ�������ch��Ϊstring���ַ����ת��Ϊc���ַ����ᵼ��'\0'û�д���
				put.push_back(',');
				itoa(_infos[i]._count, countStr, 10);

				put += countStr;
				fputs(put.c_str(), fConfig);
				fputc('\n', fConfig);
			}
		}
		fclose(fin);
		fclose(fout);//�رյ��ղŴ򿪵��ļ�
		fclose(fConfig);
		return true;
	}
	bool UnCompress(const char* filename)
	{                                             
		//1.��ȡ�����ļ�   
		string ConfigFilename = filename;  
		int count = ConfigFilename.rfind('.');  
		ConfigFilename = ConfigFilename.substr(0, count);  
		string UnCompressname = ConfigFilename + ".unpress";   

		FILE* fUnCompress = fopen(UnCompressname.c_str(), "wb"); //������ѹ���ļ�  
		ConfigFilename += ".config"; 

		FILE* fconfig = fopen(ConfigFilename.c_str(),"rb");
		assert(fconfig);
		assert(fUnCompress);

		FILE* fpress = fopen(filename,"rb");//��ѹ���õ��ļ�
		assert(fpress);

		long long charCount = 0;//�ҳ��ַ����ֵĴ���

		string line;
		_ReadLine(fconfig,line);

		charCount = atoi(line.c_str());//��һ�д���ַ����ֵ�����
		line.clear();

		while(_ReadLine(fconfig,line))//�ļ������᷵��0
		{
			if(!line.empty())
			{
				unsigned char ch = line[0];//�ȶ����ַ�
				string tmp = line.substr(2);//��ǰ�����ַ����е���ʣ�µ����ַ����ֵĴ���
				//�ַ����ļ��У��������������ļ��ж������ַ�������ʽ����
				_infos[(unsigned char)ch]._count = atoi(tmp.c_str());  
				line.clear();
			}
			else//����
			{
				line += '\n';
			}
		}
		//�ٴι���huffma��
		CharInFo invalid;
		invalid._count = 0;
		HuffmanTree<CharInFo> tree(_infos,256,invalid);
		Node* root = tree.GetRoot();
		Node* cur = root;
		char ch = fgetc(fpress);//��ѹ���ļ�
		int pos = 8;
		while(1)
		{
			--pos;
			if((ch>>pos) & 1)//Ϊ1������
			{
				cur = cur->_right;
			}
			else//Ϊ0������
			{
				cur = cur->_left;
			}
			if(cur->_left==NULL && cur->_right==NULL)//���Ҷ�Ϊ�գ�Ҷ�ӣ�������
			{
				fputc(cur->_w._ch,fUnCompress);//������ѹ�����ļ���
				cur = root;//�ٴδӸ��ڵ����
				charCount--;//�ַ���������
			}
			if(pos == 0)//һ���ֽڶ��꣬���ã�����һ���ֽ�
			{
				ch = fgetc(fpress);
				pos = 8;//����
			}
			if(charCount == 0)//���ַ�����Ϊ0,��ֹͣ��
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
	void GenerateHuffmanCode(Node* cur, string code)//��ȡHuffman���뺯��
	{
		if(cur == NULL)
			return; 
		if(cur->_left == NULL && cur->_right == NULL)
		{
			_infos[(unsigned char)cur->_w._ch]._code = code;
			return;
		}
		GenerateHuffmanCode(cur->_left, code + '0');//����Ϊ0
		GenerateHuffmanCode(cur->_right, code + '1');//����Ϊ1
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
