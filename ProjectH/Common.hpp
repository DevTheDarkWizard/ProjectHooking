#ifndef COMMON_H
#define COMMON_H

#include <windows.h>
#include <string>
#include "Packet.hpp"
#include "EntityModel.h"
#include "DigimonModel.h"

enum SERVER_TYPE { LOGIN = 0, MAP, CHAT, BATTLE };

extern DWORD baseAddress;
extern DWORD mapFileAddress;
extern DWORD mapDataAddress;
extern DWORD inLoadingAddress;

extern DWORD digimon1TPAddress;
extern DWORD digimon2TPAddress;
extern DWORD digimon3TPAddress; 
extern DWORD digimon4TPAddress; 
extern DWORD digimon5TPAddress; 
extern DWORD digimon1MTPAddress;
extern DWORD digimon2MTPAddress;
extern DWORD digimon3MTPAddress;
extern DWORD digimon4MTPAddress;
extern DWORD digimon5MTPAddress;

extern unsigned int getUnixTime();
extern int WINAPI Hooked_Send(SOCKET s, const char* buf, int len, int flags);
extern char(__stdcall* callMovePlayer)(int, int);
extern void ReadDigimon(Packet& packet, DigimonModel& digimon, bool retire = false);

extern unsigned char* ReadFile(std::string filename);
extern std::string getCurrentPath();
extern std::string getCurrentDllPath();
extern void sleep(int ms);
extern void leftClick(int x, int y);
extern void rightClick(int x, int y);
extern void moveCursor(int x, int y);
extern void sendKey(char key);
extern void sendText(const char* text, int textSize);
extern void ShowPacket(const char* buf, int len);
extern double distance(int x1, int y1, int x2, int y2);
extern int randNumber(int min, int max);
extern double randDecimal(double min, double max);
extern std::string GetDirectory(std::string filename);
extern HMODULE DllHandle;

#endif