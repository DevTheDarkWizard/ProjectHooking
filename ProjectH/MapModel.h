#pragma once
#include <vector>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

#include "PacketUnpacker.h"

struct MapModel
{
	int width;
	int height;
	char *data;
	const char *fileName;

	MapModel() {}
	MapModel(int width, int height) :
		width(width), height(height)
	{
		data = new char[width*height]();
	}
	MapModel(int width, int height, char* data) :
		width(width), height(height), data(data)
	{}
	
	void ReadData(const void* pHeader, const void*pData = NULL) {
		PacketUnpacker reader;
		reader.ResetFromMemory(pHeader);

		reader.Skip(7);
		fileName = reader.GetString(21);

		width = reader.GetInt();
		height = reader.GetInt();

		data = new char[width*height]();

		reader.Skip(8);

		int flags = reader.GetInt();

		int count = 0;

		if (pData) reader.ResetFromMemory(pData);

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				reader.Skip(20);
				short flags = reader.GetShort();

				if (flags <= 0) data[count] = 0;
				else {
					
					data[count] = 1;
				}
				reader.Skip(2);
				count++;
			}
		}

		printf("LOADED MAP W: %d H: %d TotalData: %d\n", width, height, count);
	}

	void disponse() {
		delete[] data;
	}
};
