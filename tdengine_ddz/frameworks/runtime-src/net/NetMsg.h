#ifndef __NET_NETMSG_H__
#define __NET_NETMSG_H__

#include "td_proto_cpp/include/Proto.h"

//预留3个字节
#define MIN_LENGTH 12

class NetMsg : public td_proto::Buffer
{
public:
	u32 length;
	u16 seqOrFd;
	u32 cookie;
	std::string packName;

	NetMsg() : td_proto::Buffer(0), seqOrFd(0), length(0), cookie(0) {
		this->wpos(MIN_LENGTH);
		this->resize(MIN_LENGTH);
	}

	NetMsg(const NetMsg &packet) : td_proto::Buffer(packet), seqOrFd(0), cookie(0) {
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

	std::string& getPackName()
	{
		return this->packName;
	}

	int getSize() {
		return this->size();
	}

	void endSendMsg(int seq = 0) {
		this->seqOrFd = seq;

		this->length = this->size();
		this->wpos(0);
		this->write<u32>(length);
		this->write<u16>(seq);
		this->write<u16>(cookie);
		this->wpos(MIN_LENGTH);
	}

	void readHead() {
		int pos = this->rpos();
		this->rpos(0);
		this->length = this->read<u32>();
		this->seqOrFd = this->read<u16>();
		this->rpos(MIN_LENGTH);
		td_proto::Values name = td_proto::decode_str_raw(*this, td_proto::TYPE_STR);
		if (name.sub_type != td_proto::TYPE_STR) {
			this->setVaild(false);
		} else {
			this->packName = *name._str;
		}
	}

	void writeHead() {
		int pos = this->wpos();
		this->wpos(0);
		int length = this->size();
		this->length = length;
		this->write<u32>(length);
		this->write<u16>(seqOrFd);
	}


	bool checkLenVaild() {
		int pos = this->rpos();
		this->rpos(0);
		this->length = this->read<u32>();
		this->rpos(pos);
		return this->length == this->getSize();
	}

};

#endif