#include "PacketPacker.h"

void PacketPacker::SetLength(int Size) {
	pBody->length = Size;
}

void PacketPacker::Reset()
{
	if (!pBody->length) pBody->length = PACKER_MAX_LENGHT;
	pBody->error = 0;
	pBody->aBuffer = new unsigned char[pBody->length]();
	pBody->pStart = pBody->aBuffer;
	pBody->pCurrent = pBody->pStart;
	pBody->pEnd = pBody->pCurrent + pBody->length;
}

void PacketPacker::AddByte(char value) { AddParser<char>(value); }
void PacketPacker::AddShort(short value) { AddParser<short>(value); }
void PacketPacker::AddInt(int value) { AddParser<int>(value); }
void PacketPacker::AddLong(long long value) { AddParser<long long>(value); }
void PacketPacker::AddFloat(float value) { AddParser<float>(value); }
void PacketPacker::AddDouble(double value) { AddParser<double>(value); }
void PacketPacker::AddString(const char* pStr, int Size) { AddBytes(pStr, Size); }
void PacketPacker::AddRaw(const void* pData, int Size)
{
	const char * pValue = (char*)pData;
	AddBytes(pValue, Size);
}

template <typename T>
void PacketPacker::AddParser(T value)
{
	if (pBody->error) return;

	if (pBody->pCurrent + sizeof(value) > pBody->pStart + pBody->length)
	{
		pBody->error = 1;
		return;
	}

	char pData[sizeof(value)];
	int Size = sizeof(pData);
	memcpy(pData, &value, Size);
	AddRaw(pData, Size);
}
void PacketPacker::AddBytes(const char* pStr, int Length)
{
	if (!Length) Length = strlen(pStr);

	if (pBody->error) return;

	if ((pBody->pCurrent + Length) > pBody->pStart + pBody->length)
	{
		pBody->error = 1;
		return;
	}

	int decSize = Length;

	while (decSize) {
		*pBody->pCurrent++ = *pStr++;
		pBody->pEnd++;
		if (pBody->pCurrent > pBody->pStart + pBody->length) {
			pBody->error = 1;
			break;
		}
		decSize--;
	}
}
