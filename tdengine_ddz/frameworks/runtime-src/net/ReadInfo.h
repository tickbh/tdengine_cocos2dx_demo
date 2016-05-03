#ifndef _READ_INFO_H_
#define _READ_INFO_H_
#include <stdint.h>

class ReadInfo
{
public:
	ReadInfo();
	~ReadInfo();

	char* buffer;
	int msz; //message申请内存块的大小
	int nprt;//指向数据的地址
	int sz;  //buffer申请的内存大小
	int dsz; //数据块总大小

	char* getNextMessage(int& size);
	void appendBuffer(char* value, int size);
	
private:
	char* message;
	int mSkipIndex; //读取地址的偏移量
	static const int defaultSize = 10240;
	static const int headSize = 8;
	inline bool setMessageHead();
	bool setMessageInfo(int length);

	char getBufferIndex(int index);
	void setBufferIndex(int index, char value);
	void costBufferLegnth(int length);
	int getLeftBufferSize();
	void expandBuffer(int size);
};

#endif
