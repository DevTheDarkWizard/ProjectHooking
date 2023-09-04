#ifndef PACKETBODY_H
#define PACKETBODY_H

#include <iostream>

enum {
	PACKER_MAX_LENGHT = 1024 * 2
};

class PacketBody
{
	bool disponsed;
public:
	int type;
	int length;
	unsigned char* aBuffer;
	unsigned char* pStart;
	unsigned char* pCurrent;
	unsigned char* pEnd;
	int error;

	void Skip(int pos) {
		if (error) return;

		if (pEnd && (pos < 0 || pCurrent + pos > pEnd))
		{
			error = 1;
			return;
		}

		pCurrent += pos;
	}

	void Seek(int pos) {
		if (error) return;

		if (pEnd && (pos < 0 || pStart + pos > pEnd))
		{
			error = 1;
			return;
		}

		pCurrent = pStart + pos;
	}

	void disponse() {
		if (disponsed) return;
		disponsed = true;

		pStart = 0;
		pCurrent = 0;
		pEnd = 0;
		delete[] aBuffer;
	}
};

#endif
