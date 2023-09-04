
#ifndef PACKET_H
#define PACKET_H

#include "PacketPacker.h"
#include "PacketUnpacker.h"

class Packet
{
	PacketBody pBody;
	PacketPacker pPacker;
	PacketUnpacker pUnpacker;

public:

	Packet() :
		pPacker(&pBody),pUnpacker(&pBody) {
	}
	Packet(int type) :
		pPacker(&pBody), pUnpacker(&pBody) {
		pBody.type = type;
	}
	Packet(int type, int length) :
		pPacker(&pBody), pUnpacker(&pBody) {
		pBody.type = type;
		pBody.length = length;
	}
	Packet(const char* pData, int Size) :
		pPacker(&pBody), pUnpacker(&pBody) {
		pBody.length = *(int*)(pData + 4);
		Reset(pData, Size);
	}
	Packet(int type, const char* pData, int Size) :
		pPacker(&pBody), pUnpacker(&pBody) {
		pBody.length = *(int*)(pData + 4);
		Reset(pData, Size, type);
	}

	void Reset(const char* pData, int Size = -1, int type = -1)
	{
		if (type == -1) pBody.type = *(short*)(pData + 1);
		else pBody.type = type;
		pBody.length = *(int*)(pData + 4);
		pUnpacker.Reset(pData, Size);
		
	}

	void Skip(int pos) {
		pBody.Skip(pos);
	}

	void Seek(int pos) {
		pBody.Seek(pos);
	}

	inline const char* Data() const { return pUnpacker.Data(); }
	inline int GetType() const { return pBody.type; }
	inline int GetLength() const { return pBody.length; }
	inline int GetTotalLength() const { return pUnpacker.Size(); }

	void SetLength(int Size) { 
		pBody.length = Size; 
		pBody.pEnd = pBody.pStart + pBody.length;
	}

	void AddLength(int Size) { 
		pBody.length += Size; 
		pBody.pEnd += Size;
	}

	inline PacketPacker& Writer() { return pPacker; }
	inline PacketUnpacker& Reader() { return pUnpacker; }

	void disponse() {
		pBody.disponse();
	}
};

#endif
