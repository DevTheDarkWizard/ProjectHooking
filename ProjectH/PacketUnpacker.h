#ifndef PACKET_UNPACKER_H
#define PACKET_UNPACKER_H

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

#include "PacketBody.h"

class PacketUnpacker
{
	PacketBody* pBody;
public:
	PacketUnpacker() {
		pBody = new PacketBody();
	}

	PacketUnpacker(PacketBody* body) :
		pBody(body) {
	}

	void Reset(const void* pData, int Size);
	void ResetFromMemory(const void* pData);
	char GetByte(int Offset = -1);
	short GetShort(int Offset = -1);
	int GetInt(int Offset = -1);
	long long GetLong(int Offset = -1);
	float GetFloat(int Offset = -1);
	double GetDouble(int Offset = -1);
	const char* GetString(int Size, int Offset = -1);
	const char* GetBytes(int Size);
	const char* GetBytesOffset(int Size, int Offset);
	const unsigned char* GetRaw(int Size);

	template <typename T>
	T GetParser();
	template <typename T>
	T GetParser(int Offset);

	void SetLength(int Size);

	int Position() const { return (int)(pBody->pCurrent - pBody->pStart); }
	int Size() const { return (int)(pBody->pEnd - pBody->pStart); }
	const char* Data() const { return (const char*)pBody->aBuffer; }
	bool Error() const { return pBody->error; }

	inline void Skip(int pos) { pBody->Skip(pos); }
	inline void Seek(int pos) { pBody->Seek(pos); }
};

#endif

