#include "ReadInfo.h"
#include <assert.h>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include "NetMsg.h"
#include "cocos2d.h"
using namespace td_proto;


ReadInfo::ReadInfo()
:buffer(new char[defaultSize]), message(new char[defaultSize]), nprt(0), sz(defaultSize), dsz(0), msz(defaultSize), mSkipIndex(0)
{

	memset(buffer, 0, defaultSize);
	memset(message, 0, defaultSize);
}

ReadInfo::~ReadInfo()
{
	delete[] buffer;
	delete[] message;
}

char* ReadInfo::getNextMessage( int& size ) {
	mSkipIndex = 0;
	if(!setMessageHead()) {
		return nullptr;
	}
	u32 messageLength = ByteGetValue<u32>(message);
	messageLength = messageLength ^ NET_DATA_LEN_XOR;

	if(messageLength == 0 || messageLength < headSize) {
		costBufferLegnth(headSize);
		cocos2d::log("!!!!!!!!!!!!!!!length!!!!!!!!!!! message length is %d", messageLength);
		return nullptr;
	}
	size = messageLength;
	mSkipIndex = 0;
	if(!setMessageInfo(messageLength)) {
		return nullptr;
	}
	costBufferLegnth(messageLength);
	return message;
}


void ReadInfo::appendBuffer( char* value, int size )
{
	int left = getLeftBufferSize();
	if(size > left) {
		expandBuffer(sz + size);
	}
	int cprt = (nprt + dsz) % sz;
	if (cprt + size < sz) {
		memcpy(buffer + cprt, value, size);
	} else {
		int first = sz - cprt;
		memcpy(buffer + cprt, value, first);
		memcpy(buffer, value + first, size - first);
	}

	dsz += size;
}

bool ReadInfo::setMessageHead()
{
	return setMessageInfo(headSize);
}

bool ReadInfo::setMessageInfo( int length )
{
	if (length < 0) {
		return false;
	}
	if(dsz < length + mSkipIndex) {
		return false;
	}
	if(msz < length) {
		delete[] message;
		msz = (int)(length * 1.2f);
		message = new char[msz];
		cocos2d::log("auto expand message size to %d", msz);
	}
	int cprt = nprt + mSkipIndex;
	if (cprt + length < sz) {
		memcpy(message, buffer + cprt, length);
	} else {
		int first = sz - cprt;
		memcpy(message, buffer + cprt, first);
		memcpy(message + first, buffer, length - first);
	}
	return true;
}

char ReadInfo::getBufferIndex( int index )
{
	assert(index < sz);
	if(index >= dsz) {
		return 0;
	}
	return buffer[(nprt + index) % sz];
}

void ReadInfo::setBufferIndex( int index, char value )
{
	assert(index < sz);
	buffer[(nprt + index) % sz] = value;
}


void ReadInfo::costBufferLegnth( int length )
{
	assert(length <= dsz);
	dsz -= length;
	nprt = (nprt + length) % sz;
}

int ReadInfo::getLeftBufferSize()
{
	return sz - dsz;
}

void ReadInfo::expandBuffer(int size)
{
	int newSize = (int)(size * 1.2f);
	char* newBuffer = new char[newSize];
	memset(newBuffer, 0, newSize);
	memcpy(newBuffer, buffer + nprt, (sz - nprt));
	if (nprt > 0) {
		memcpy(newBuffer + (sz - nprt), buffer, nprt);
	}
	delete[] buffer;
	buffer = newBuffer;
	nprt = 0;
	sz = newSize;
}
