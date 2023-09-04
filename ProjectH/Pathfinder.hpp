#ifndef PATHFINDER_H
#define PATHFINDER_H 

#include <fstream>
#include <map>
#include <mutex>

#include "Common.hpp"
#include "Pathfinders.h"
#include "MapModel.h"


class Pathfinder
{
	std::map<int, MapModel*> _maps;
	std::mutex _pathfinderMutex;

	int _currentMap;
	std::vector<std::tuple<int, int>> _paths;

	int lastSX;
	int lastSY;
	int lastEX;
	int lastEY;
	
public:
	Pathfinder() :
		_currentMap(0), _paths(std::vector<std::tuple<int, int>>()) {}

	std::vector<std::tuple<int, int>> find(int sX, int sY, int eX, int eY)
	{
		if (!_currentMap || !_maps.size() || _maps.find(_currentMap) == _maps.end()) 
			return std::vector<std::tuple<int, int>>();

		std::lock_guard<std::mutex> lock(_pathfinderMutex);

		_paths.clear();

		auto it = _maps.find(_currentMap);

		if (it != _maps.end()) {

			const unsigned char* data = (const unsigned char*)it->second->data;
			const int width = it->second->width;
			const int height = it->second->height;
			const int length = it->second->width * it->second->height;

			if (sX < 0) sX = 0;
			if (sY < 0) sY = 0;
			if (eX < 0) eX = 0;
			if (eY < 0) eY = 0;

			if (sX > width) sX = width;
			if (sY > height) sY = height;
			if (eX > width) eX = width;
			if (eY > height) eY = height;

			lastSX = sX;
			lastSY = sY;
			lastEX = eX;
			lastEY = eY;

			int* buffer = new int[length + 1]();

			int result = AStarFindPathDiag(sX, sY, eX, eY, data, width, height, buffer, length + 1);

			_paths = convertPaths(width, height, buffer, length + 1, result);

			delete[] buffer;

			return _paths;
		}
		return std::vector<std::tuple<int, int>>();
	}

	inline std::vector<std::tuple<int, int>>& getPaths() { return _paths; }

	void setMap(int map) {
		_currentMap = map;
	}

	std::tuple<int, int> getMapDimensions(int map) {
		std::lock_guard<std::mutex> lock(_pathfinderMutex);

		auto it = _maps.find(_currentMap);

		if (it != _maps.end()) {
			return std::tuple<int,int>(it->second->width, it->second->height);
		}

		return std::tuple<int, int>();
	}

	void clearMaps() {
		std::lock_guard<std::mutex> lock(_pathfinderMutex);

		for (auto&& it : _maps)
		{
			it.second->disponse();
			delete it.second;
		}

		_maps.clear();
	}

	bool checkCanMove(int x, int y) {
		std::lock_guard<std::mutex> lock(_pathfinderMutex);

		auto it = _maps.find(_currentMap);

		if (it != _maps.end()) {
			bool ret = ((it->second->data[it->second->width * y + x]) ? true : false);
			//printf("ret: %d\n", ret);
			return ret;
		}

		return false;
	}

	void removeMap(int id) {
		std::lock_guard<std::mutex> lock(_pathfinderMutex);

		auto it = _maps.find(_currentMap);

		if (it != _maps.end()) {
			it->second->disponse();
			delete it->second;
			_maps.erase(id);
		}
	}

	void addMap(int id, MapModel* map) {

		std::lock_guard<std::mutex> lock(_pathfinderMutex);

		auto it = _maps.find(id);

		if (it == _maps.end())
		{
			printf("ADD MAP ID: %d\n", id);
			_maps.emplace(id, map);
		}
	}

	void addMap(MapModel* map) {

		addMap(_currentMap, map);
	}

	void addMap(int id, int width, int height, unsigned char* data) {

		std::lock_guard<std::mutex> lock(_pathfinderMutex);

		auto it = _maps.find(id);

		if (it == _maps.end())
		{
			auto map = new MapModel();

			map->width = width;
			map->height = height;

			int length = map->width * map->height;

			map->data = new char[length]();
			memcpy(map->data, (const char*)data, length);

			delete[] data;

			_maps.emplace(id, map);
		}
	}

	void Log() {
		std::lock_guard<std::mutex> lock(_pathfinderMutex);

		auto it = _maps.find(_currentMap);

		std::ofstream fw(getCurrentDllPath() + "\\map.txt", std::ofstream::out);

		if (fw.is_open())
		{
			if (it != _maps.end()) {
				for (int i = 0; i < it->second->width * it->second->height; i++) {
					if (i > 1 && i % it->second->width == 0) fw << std::endl;
					int y = (i / it->second->width);
					int x = i - (y * it->second->width);
					bool contains = std::find(_paths.begin(), _paths.end(), std::tuple<int, int>(x, y)) != _paths.end();
					//printf("%d,%d\n", x, y);
					if (x == lastSX && y == lastSY) 
						fw << 'S';
					else if (x == lastEX && y == lastEY)
						fw << 'F';
					else if (contains)
						fw << '#';
					else fw << (it->second->data[i] ? '.' : 'O');
				}

				fw.close();
			}
		}
	}

	void disponse() {
		clearMaps();

		_paths.clear();
	}

private:

	std::vector<std::tuple<int, int>> convertPaths(int width, int height,int * buffer, int bufferSize, int result) {
		std::vector<std::tuple<int, int>> ret;

		if (result > 0) {
			for (int i = 0; i < bufferSize; i++) {
				int value = buffer[i];
				if (value > 0) {
					int y = buffer[i] / width;
					int x = buffer[i] - (y * width);
					if (x >= 0 && x < width && y >= 0 && y < height) ret.push_back(std::tuple<int, int>(x, y));
					if (x == lastEX && y == lastEY) break;
				}
			}
		}

		return ret;
	}
};

#endif
