#ifndef PACKET_PACKER_H
#define PACKET_PACKER_H

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include "PacketBody.h"

class PacketPacker
{
	PacketBody* pBody;
public:
	PacketPacker() {
		pBody = new PacketBody();
		Reset();
	}
	PacketPacker(short type, short length) {
		pBody = new PacketBody();
		pBody->type = type;
		pBody->length = length;
		Reset();
	}
	PacketPacker(PacketBody* body) :
		pBody(body) {
	}

	void Reset();
	void AddByte(char value);
	void AddShort(short value);
	void AddInt(int value);
	void AddLong(long long value);
	void AddFloat(float value);
	void AddDouble(double value);
	void AddString(const char* pStr, int Size = 0);
	void AddBytes(const char* pStr, int Size = 0);
	void AddRaw(const void* pData, int Size = 0);

	template <typename T>
	void AddParser(T value);

	void SetLength(int Size);

	int Position() const { return (int)(pBody->pCurrent - pBody->pStart); }
	int Size() const { return (int)(pBody->pCurrent - pBody->aBuffer); }
	const char* Data() const { return (const char*)pBody->aBuffer; }
	bool Error() const { return pBody->error; }

	inline void Skip(int pos) { pBody->Skip(pos); }
	inline void Seek(int pos) { pBody->Seek(pos); }
};

#endif

