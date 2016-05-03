#ifndef __NET_NETMSG_H__
#define __NET_NETMSG_H__

#include "td_proto_cpp/include/Buffer.h"

#define NET_DATA_LEN_XOR	0x12345678

#define DEFAULT_SIGN_INFO 0x9876
//预留3个字节
#define MIN_LENGTH 12

class NetMsg : public td_proto::Buffer
{
public:
	u16 packId;
	u16 seqOrFd;
	u32 lengthPos;
	u16 signInfo;
	u8  isCompress;

	NetMsg() : td_proto::Buffer(0), packId(0), seqOrFd(0), lengthPos(0), signInfo(DEFAULT_SIGN_INFO), isCompress(0) {
		this->wpos(MIN_LENGTH);
		this->resize(MIN_LENGTH);
	}
	NetMsg(const NetMsg &packet) : td_proto::Buffer(packet), packId(0), seqOrFd(0), signInfo(DEFAULT_SIGN_INFO), isCompress(0)   {
	}
	NetMsg(const char* message, int size)  {
		append(message, size);
		readHead();
	}
	~NetMsg() {
	}

	int getOpFd() {
		return this->seqOrFd;
	}

	void setOpFd(int fd) {
		this->seqOrFd = fd;
		int pos = this->wpos();
		this->wpos(4);
		this->write<u16>(fd);
		this->wpos(pos);
	}

	int getPackId()
	{
		return packId;
	}

	int getSize() {
		return this->size();
	}

	void endSendMsg(int id, int seq = 0) {
		this->packId = id;
		this->seqOrFd = seq;
		int length = this->size();
		if (length < MIN_LENGTH) {
			length = MIN_LENGTH;
			this->resize(MIN_LENGTH);
		}
		this->lengthPos = length;
		this->wpos(0);
		length ^= NET_DATA_LEN_XOR;
		this->write<u32>(length);
		this->write<u16>(seq);
		this->write<u16>(id);
		this->write<u16>(signInfo);
		this->write<u8>(isCompress);
		this->wpos(MIN_LENGTH);
	}

	void setIsCompress(u8 isCompress) {
		this->isCompress = isCompress;
		int wpos = this->wpos();
		this->wpos(10);
		this->write<u8>(isCompress);
		this->wpos(wpos);
	}

	u8 getIsCompress() {
		return this->isCompress;
	}

	void writeString(const char *str)
	{
		if (!str) str = "";
		u16 szLen = (u16)strlen(str);
		append<u16>(szLen);
		append(str, (size_t)szLen);
	}

	void writeRaw(const char *str, u32 len)
	{
		if (!str) {
			len = 0;
			str = "";
		}
		append<u32>(len);
		append(str, (size_t)len);
	}

	void writeBinary(const char* str, int size) {
		this->wpos(0);
		append(str, size);
		readHead();
	}

	u8* readRaw(u32& len) {
		len = this->read<u32>();
		if (len > 1000000 || len > this->size()) {
			len = 1;
		}
		u8* value = new u8[len];
		this->read(value, len);
		return value;
	}

	void readHead() {
		int pos = this->rpos();
		this->rpos(0);
		lengthPos = this->read<u32>();
		lengthPos ^= NET_DATA_LEN_XOR;
		seqOrFd = this->read<u16>();
		packId = this->read<u16>();
		signInfo = this->read<u16>();
		isCompress = this->read<u8>();
		this->rpos(MIN_LENGTH);
	}

	void writeHead() {
		int pos = this->wpos();
		this->wpos(0);
		int length = this->size();
		if (length < MIN_LENGTH) {
			length = MIN_LENGTH;
			this->resize(MIN_LENGTH);
		}
		this->lengthPos = length;
		length ^= NET_DATA_LEN_XOR;
		this->write<u32>(length);
		this->write<u16>(seqOrFd);
		this->write<u16>(packId);
		this->write<u16>(signInfo);
		this->write<u8>(isCompress);
	}

	void signMsg() {
		u16 sign = DEFAULT_SIGN_INFO;
		for (int i = MIN_LENGTH; i < this->getSize(); ++i)
		{
			sign += this->at(i);
		}
		int pos = this->wpos();
		this->wpos(8);
		this->write<u16>(sign);
		this->wpos(pos);
	}

	bool checkSign() {
		u16 sign = DEFAULT_SIGN_INFO;
		for (int i = MIN_LENGTH; i < this->getSize(); ++i)
		{
			sign += this->at(i);
		}
		int pos = this->rpos();
		this->rpos(8);
		u16 netSign = this->read<u16>();
		this->rpos(pos);
		return sign == netSign;
	}

	bool checkLenVaild() {
		int pos = this->rpos();
		this->rpos(0);
		lengthPos = this->read<u32>();
		lengthPos ^= NET_DATA_LEN_XOR;
		this->rpos(pos);
		return lengthPos == this->getSize();
	}

	std::string readString()
	{
		u32 szLen = read<u16>();
		std::string str;
		unsigned int i = 0;
		while (i++ < szLen)
		{
			char c = read<char>();
			if (c == 0)
				break;

			str += c;
		}

		return str;
	}

};

#endif