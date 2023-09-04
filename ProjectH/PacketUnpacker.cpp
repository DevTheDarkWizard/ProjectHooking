#include "PacketUnpacker.h"

void PacketUnpacker::SetLength(int Size) {
	pBody->length = Size;
}

void PacketUnpacker::Reset(const void* pData, int Size)
{
	if (!pBody->length) pBody->length = PACKER_MAX_LENGHT;
	pBody->error = 0;
	pBody->aBuffer = new unsigned char[pBody->length]();
	memcpy(pBody->aBuffer, (unsigned char*)pData, Size);
	pBody->pStart = pBody->aBuffer;
	pBody->pEnd = pBody->pStart + Size;
	pBody->pCurrent = pBody->pEnd;
}

void PacketUnpacker::ResetFromMemory(const void* pData)
{
	pBody->error = 0;
	pBody->pStart = *(unsigned char**)pData;
	pBody->pCurrent = pBody->pStart;
}

template <typename T>
T PacketUnpacker::GetParser()
{
	if (pBody->error)
		return 0;

	if (pBody->pEnd && (pBody->pCurrent > pBody->pStart + pBody->length || pBody->pCurrent + sizeof(T) > pBody->pStart + pBody->length))
	{
		pBody->error = 1;
		return 0;
	}

	std::istringstream is((char*)pBody->pCurrent);
	T ret;
	is >> ret;

	pBody->pCurrent += sizeof(T);

	if (pBody->pEnd && pBody->pCurrent > pBody->pStart + pBody->length)
	{
		pBody->error = 1;
		return 0;
	}

	return ret;
}
template <typename T>
T PacketUnpacker::GetParser(int Offset)
{
	if (pBody->error)
		return 0;

	if (pBody->pEnd && pBody->pStart + Offset + sizeof(T) > pBody->pStart + pBody->length)
	{
		pBody->error = 1;
		return 0;
	}

	T ret = (T*)(pBody->pStart + Offset);

	return ret;
}

char PacketUnpacker::GetByte(int Offset) {
	if (pBody->error)
		return 0;

	if (pBody->pEnd && (pBody->pCurrent >= pBody->pStart + pBody->length ||
		pBody->pCurrent + sizeof(char) > pBody->pStart + pBody->length ||
		Offset >= 0 && pBody->pStart + Offset + sizeof(char) > pBody->pStart + pBody->length)) {
		pBody->error = 1;
		return 0;
	}

	char ret;
	if (Offset >= 0) ret = *(char*)(pBody->pStart + Offset);
	else {
		ret = *(char*)pBody->pCurrent;

		pBody->pCurrent += sizeof(char);

		if (pBody->pEnd && pBody->pEnd && pBody->pCurrent > pBody->pStart + pBody->length) {
			pBody->error = 1;
			return 0;
		}
	}

	return ret;
}
short PacketUnpacker::GetShort(int Offset) {
	if (pBody->error)
		return 0;

	if (pBody->pEnd && (pBody->pCurrent >= pBody->pStart + pBody->length ||
		pBody->pCurrent + sizeof(short) > pBody->pStart + pBody->length ||
		Offset >= 0 && pBody->pStart + Offset + sizeof(short) > pBody->pStart + pBody->length)) {
		pBody->error = 1;
		return 0;
	}

	short ret;
	if (Offset >= 0) ret = *(short*)(pBody->pStart + Offset);
	else {
		ret = *(short*)pBody->pCurrent;

		pBody->pCurrent += sizeof(short);

		if (pBody->pEnd && pBody->pCurrent > pBody->pStart + pBody->length) {
			pBody->error = 1;
			return 0;
		}
	}

	return ret;
}
int PacketUnpacker::GetInt(int Offset) {
	if (pBody->error)
		return 0;

	if (pBody->pEnd && (pBody->pCurrent >= pBody->pStart + pBody->length ||
		pBody->pCurrent + sizeof(int) > pBody->pStart + pBody->length ||
		Offset >= 0 && pBody->pStart + Offset + sizeof(int) > pBody->pStart + pBody->length)) {
		pBody->error = 1;
		return 0;
	}

	int ret;
	if (Offset >= 0) ret = *(int*)(pBody->pStart + Offset);
	else {
		ret = *(int*)pBody->pCurrent;

		pBody->pCurrent += sizeof(int);

		if (pBody->pEnd && pBody->pCurrent > pBody->pStart + pBody->length) {
			pBody->error = 1;
			return 0;
		}
	}

	return ret;
}
long long PacketUnpacker::GetLong(int Offset) {
	if (pBody->error)
		return 0;

	if (pBody->pEnd && (pBody->pCurrent >= pBody->pStart + pBody->length ||
		pBody->pCurrent + sizeof(long long) > pBody->pStart + pBody->length ||
		Offset >= 0 && pBody->pStart + Offset + sizeof(long long) > pBody->pStart + pBody->length)) {
		pBody->error = 1;
		return 0;
	}

	long long ret;
	if (Offset >= 0) ret = *(long long*)(pBody->pStart + Offset);
	else {
		ret = *(long long*)pBody->pCurrent;

		pBody->pCurrent += sizeof(long long);

		if (pBody->pEnd && pBody->pCurrent > pBody->pStart + pBody->length) {
			pBody->error = 1;
			return 0;
		}
	}

	return ret;
}
float PacketUnpacker::GetFloat(int Offset) {
	if (pBody->error)
		return 0;

	if (pBody->pEnd && (pBody->pCurrent >= pBody->pStart + pBody->length ||
		pBody->pCurrent + sizeof(float) > pBody->pStart + pBody->length ||
		Offset >= 0 && pBody->pStart + Offset + sizeof(float) > pBody->pStart + pBody->length)) {
		pBody->error = 1;
		return 0;
	}

	float ret;
	if (Offset >= 0) ret = *(float*)(pBody->pStart + Offset);
	else {
		ret = *(float*)pBody->pCurrent;

		pBody->pCurrent += sizeof(float);

		if (pBody->pEnd && pBody->pCurrent > pBody->pStart + pBody->length) {
			pBody->error = 1;
			return 0;
		}
	}

	return ret;
}
double PacketUnpacker::GetDouble(int Offset) {
	if (pBody->error)
		return 0;

	if (pBody->pEnd && (pBody->pCurrent >= pBody->pStart + pBody->length ||
		pBody->pCurrent + sizeof(double) > pBody->pStart + pBody->length ||
		Offset >= 0 && pBody->pStart + Offset + sizeof(double) > pBody->pStart + pBody->length)) {
		pBody->error = 1;
		return 0;
	}

	double ret;
	if (Offset >= 0) ret = *(double*)(pBody->pStart + Offset);
	else {
		ret = *(double*)pBody->pCurrent;

		pBody->pCurrent += sizeof(double);

		if (pBody->pEnd && pBody->pCurrent > pBody->pStart + pBody->length) {
			pBody->error = 1;
			return 0;
		}
	}

	return ret;
}

const char* PacketUnpacker::GetString(int Size, int Offset) { return Offset == -1 ? GetBytes(Size) : GetBytesOffset(Size, Offset); }
const char* PacketUnpacker::GetBytes(int Size)
{
	if (pBody->error ||
		pBody->pEnd && 
		(pBody->pCurrent > pBody->pStart + pBody->length ||
		pBody->pCurrent + Size > pBody->pStart + pBody->length))
		return "";

	//char* pPtr = (char*)malloc(Size);
	char* pPtr = new char[Size]();
	memcpy(pPtr, (const char*)pBody->pCurrent, Size);
	pBody->pCurrent += Size;

	return (const char*)pPtr;
}
const char* PacketUnpacker::GetBytesOffset(int Size, int Offset)
{
	if (pBody->error ||
		pBody->pEnd &&
		(pBody->pStart + Offset + Size > pBody->pStart + pBody->length))
		return "";

	char* pPtr = new char[Size]();
	memcpy(pPtr, (const char*)(pBody->pStart + Offset), Size);

	return (const char*)pPtr;
}
const unsigned char* PacketUnpacker::GetRaw(int Size)
{
	if (pBody->error)
		return 0;

	if (Size < 0 || pBody->pEnd && pBody->pCurrent + Size > pBody->pStart + pBody->length)
	{
		pBody->error = 1;
		return 0;
	}

	const unsigned char* pPtr = pBody->pCurrent;

	pBody->pCurrent += Size;
	return pPtr;
}

