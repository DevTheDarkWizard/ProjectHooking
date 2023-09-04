#include <winsock2.h>
#include <ws2tcpip.h>
#include <list>
#include <tuple>
#include <string>
#include <windows.h>
#include <detours.h>
#include <thread>
#include <math.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <cstdlib>

#include "MapModel.h"

#include "Packet.hpp"
#include "Packets.h"

#include "Common.hpp"
#include "PlayerModule.h"
#include "LootModule.h"
#include "AttackModule.h"
#include "MoveModule.h"
#include "Sender.h"

#pragma comment( lib, "Ws2_32.lib" )
#pragma comment( lib, "detours.lib" )

//#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)    // only show filename and not it's path (less clutter)
//#define INFO std::cout << std::put_time(std::localtime(&time_now), "%y-%m-%d %OH:%OM:%OS") << " [INFO] " << __FILENAME__ << "(" << __FUNCTION__ << ":" << __LINE__ << ") >> "
//#define ERROR std::cout << std::put_time(std::localtime(&time_now), "%y-%m-%d %OH:%OM:%OS") << " [ERROR] " << __FILENAME__ << "(" << __FUNCTION__ << ":" << __LINE__ << ") >> "


//512

#define DATA_BUFSIZE 1820

typedef struct _SOCKET_INFORMATION {
	OVERLAPPED Overlapped;
	SOCKET Socket;
	CHAR Buffer[DATA_BUFSIZE];
	WSABUF DataBuf;
	DWORD BytesSEND;
	DWORD BytesRECV;
} SOCKET_INFORMATION, *LPSOCKET_INFORMATION;

WSAOVERLAPPED Overlapped;

DWORD baseAddress = 0x400000;
//DigitalWorldW2.exe+EE9984 loaded, zero in seconds after load
DWORD inLoadingAddress = 0x106821D;
DWORD mapFileAddress = 0x649434;
DWORD mapDataAddress = 0x0249464;//Offset 8

DWORD digimon1TPAddress = 0x67E5DC;//+36 = MTP
DWORD digimon2TPAddress = 0x6836AC;// + 0x970 para os pr�ximos digis
DWORD digimon3TPAddress = digimon2TPAddress + 0x970;
DWORD digimon4TPAddress = digimon3TPAddress + 0x970;
DWORD digimon5TPAddress = digimon4TPAddress + 0x970;
DWORD digimon1MTPAddress = digimon1TPAddress + 36;
DWORD digimon2MTPAddress = digimon2TPAddress + 36;
DWORD digimon3MTPAddress = digimon3TPAddress + 36;
DWORD digimon4MTPAddress = digimon4TPAddress + 36;
DWORD digimon5MTPAddress = digimon5TPAddress + 36;

//DWORD mapDataAddress2 = 0x249464;//Offset 0

HWND mainHandler;
HWND mainHandler2;
HWND editHandler;

#include "WinFunctions.hpp"
FILE* fDummy;

bool threadCreated = false;
bool running = false;

std::thread inputThread;
std::thread botThread;

std::list<Module*> _modules;
PlayerModule* _player;
LootModule* _loot;
MoveModule* _move;
AttackModule* _attack;

HMODULE DllHandle;
//sub_4187C0
 
//sub_430690 ?? testar (draw outros players) chama sub_42A4C0
//sub_42A970 escreve outros players na tela  esse chama sub_42A4C0
//sub_42D450 esse chama sub_42A4C0
//sub_42A4C0 //??
//sub_42D9A0 draw frame in screen

//sub_42D5E0


//sub_48FEF0 main handler dbo
//sub_41BA30 event handler (mouse,keyboard)
//sub_41C170 ChatEditpro keydown
//sub_41B7D0 create window
//string to find it "Server Battle Finish %d\n"

Packet* packetSend;
Packet* packetRecv;

bool showRecv = true;
bool showSend = true;

//int sendIgnore[] = { 0 };
//int recvIgnore[] = { 0 };

int sendIgnore[] = {
	222,
	221,
	180,
	145,
	144,
	106,
	105,
	102,
	101,
	100,
	98,
	97,
	44,
	33,
	32,
	22,
	20,
	18,
	12,
	4,
	3,
	1,
	0
};
int recvIgnore[] = {
	222,
	197,
	180,
	115,
	178,
	176,
	145,
	144,
	129,
	112,
	110,
	109,
	107,
	105,
	104,
	103,
	101,
	100,
	99,
	99,
	98,
	97,
	33,
	32,
	22,
	20,
	19,//Item on floor
	18,
	17,
	16,
	5,
	4,
	3,
	2,
	0
};

extern std::list<Module*> _modules;

void Exit();
void CreateThreads();
void CreateConsole();
void processInput();
void BotLoop();
int SendFunction(SOCKET socket, const char* buf, int len, int flags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
void RecvFunction(const char* buf, int len);

int (WINAPI *Original_Send)(SOCKET s, const char *buf, int len, int flags) = send;
int (WINAPI *Original_Recv)(SOCKET s, char *buf, int len, int flags) = recv;
int (WINAPI *Original_WSASend)(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, DWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) = WSASend;
int (WINAPI *Original_WSARecv)(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) = WSARecv;
void (WINAPI *Original_Outputdebug)(LPCSTR lpOutputString) = OutputDebugStringA;
int (WSAAPI * Original_Connect)(SOCKET s, const sockaddr *name, int namelen) = connect;
int (WINAPI * Original_Bind)(SOCKET s, const sockaddr *addr, int namelen) = bind;
void (WINAPI * Original_ExitProcess)(UINT uExitCode) = ExitProcess;

void CALLBACK WorkerRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags);

int WINAPI Hooked_Send(SOCKET s, const char* buf, int len, int flags);
int WINAPI Hooked_Recv(SOCKET s, char *buf, int len, int flags);
int WINAPI Hooked_WSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, DWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
int WINAPI Hooked_WSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
int WSAAPI Hooked_Connect(SOCKET s, const sockaddr *name, int namelen);
int WINAPI Hooked_Bind(SOCKET s, const sockaddr *addr, int namelen);
void WINAPI Hooked_ExitProcess(UINT uExitCode);
void WINAPI Hooked_Outputdebug(LPCSTR lpOutputString);

//int(__stdcall* sub_487100)() = (int(__stdcall*)())(0x487100);
//int(__stdcall* sub_437EE0)() = (int(__stdcall*)())(0x437EE0);
//int(__stdcall* sub_4162A0)(void *) = (int(__stdcall*)(void *))(0x4162A0);
//sub_438170 mouse calls handler

int*(__stdcall* sub_423CC0)() = (int*(__stdcall*)())(0x423CC0);
int*(__stdcall* sub_419AC0)() = (int*(__stdcall*)())(0x419AC0);
int(__stdcall* sub_419C40)(int*) = (int(__stdcall*)(int*))(0x419C40);
int(__stdcall* sub_5132E0)() = (int(__stdcall*)())(0x5132E0);
int(__stdcall* sub_5158C0)(int) = (int(__stdcall*)(int))(0x5158C0); 
char(__stdcall* callMovePlayer)(int, int) = (char(__stdcall*)(int, int))(0x4398E0);



void CALLBACK WorkerRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags)
{
	DWORD RecvBytes, SendBytes;
	DWORD Flags;

	LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION)Overlapped;

	if (Error != 0 || BytesTransferred == 0)
	{
		closesocket(SI->Socket);
		GlobalFree(SI);
		return;
	}

	if (SI->BytesRECV == 0)
	{
		SI->BytesRECV = BytesTransferred;
		SI->BytesSEND = 0;
	}
	else
	{
		SI->BytesSEND += BytesTransferred;
	}

	if (SI->BytesRECV > SI->BytesSEND)
	{
		ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));
		SI->DataBuf.buf = SI->Buffer + SI->BytesSEND;
		SI->DataBuf.len = SI->BytesRECV - SI->BytesSEND;

		if (WSASend(SI->Socket, &(SI->DataBuf), 1, &SendBytes, 0, &(SI->Overlapped), WorkerRoutine) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				printf("WSASend() failed with error %d\n", WSAGetLastError());
				return;
			}
		}
		else
		{
			printf("SEND:\n");
			ShowPacket(SI->DataBuf.buf, SendBytes);
		}
	}
	else
	{
		SI->BytesRECV = 0;
		Flags = 0;
		//ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));
		//SI->DataBuf.len = DATA_BUFSIZE;
		//SI->DataBuf.buf = SI->Buffer;
		ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));
		SI->DataBuf.buf = SI->Buffer + SI->BytesSEND;
		SI->DataBuf.len = SI->BytesRECV - SI->BytesSEND;

		if (WSARecv(SI->Socket, &(SI->DataBuf), 1, &RecvBytes, &Flags, &(SI->Overlapped), WorkerRoutine) == SOCKET_ERROR)
		{

			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				printf("WSARecv() failed with error %d\n", WSAGetLastError());
				return;
			}
		}
		else {
			printf("RECV:\n");
			ShowPacket(SI->DataBuf.buf, RecvBytes);
		}
	}
}

int WINAPI Hooked_Bind(SOCKET s, const sockaddr *addr, int namelen) {
	CreateThreads();
	printf("Binding!\n");
	return Original_Bind(s, addr, namelen);
}

int WSAAPI Hooked_Connect(SOCKET s, const sockaddr *name, int namelen) {
	CreateThreads();

	char ip[INET6_ADDRSTRLEN > INET_ADDRSTRLEN ? INET6_ADDRSTRLEN : INET_ADDRSTRLEN] = { '\0' };
	if (name->sa_family == AF_INET)
	{
		struct sockaddr_in *addr_in = (struct sockaddr_in *)name;
		inet_ntop(AF_INET, &(addr_in->sin_addr), ip, INET_ADDRSTRLEN);
	}
	else if (name->sa_family == AF_INET6) {
		struct sockaddr_in6 *addr_in6 = (struct sockaddr_in6 *)name;
		inet_ntop(AF_INET6, &(addr_in6->sin6_addr), ip, INET6_ADDRSTRLEN);
	}

	unsigned short port = (name->sa_data[0] << 8) | name->sa_data[1];

	printf("Connecting to %s:%d\n", &ip, port);

	if (port == 7200) Sender::Get()->setSendSocketLogin(s);
	if (port == 7201) Sender::Get()->setSendSocketChat(s);
	if (port == 7001) Sender::Get()->setSendSocketMap(s);
	if (port == 7002) Sender::Get()->setSendSocketBattle(s);

	return Original_Connect(s, name, namelen);
}

LPSOCKET_INFORMATION SocketInfo;

int WINAPI Hooked_WSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, DWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	
	std::lock_guard<std::mutex> lock(Sender::Get()->acquire());

	CreateThreads();

	/*SocketInfo = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR, sizeof(SOCKET_INFORMATION));
	SocketInfo->Socket = s;

	ZeroMemory(&(SocketInfo->Overlapped), sizeof(WSAOVERLAPPED));
	DWORD test = 8;
	SocketInfo->BytesSEND = 0;
	SocketInfo->BytesRECV = 0;
	SocketInfo->DataBuf.len = DATA_BUFSIZE;
	SocketInfo->DataBuf.buf = SocketInfo->Buffer;*/
	//ret = (*Original_WSARecv)(s, &(SocketInfo->DataBuf), dwBufferCount, lpNumberOfBytesRecvd, lpFlags, &(SocketInfo->Overlapped), WorkerRoutine);

	int ret = SendFunction(s, lpBuffers->buf, lpBuffers->len, (int)lpFlags, lpOverlapped, lpCompletionRoutine);

	if (ret == 0) {
		ret = (*Original_WSASend)(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine);
	}

	return ret;
}

int WINAPI Hooked_WSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	CreateThreads();

	int ret = (*Original_WSARecv)(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine);

	RecvFunction(lpBuffers->buf, lpBuffers->len);
	
	return ret;
}

int WINAPI Hooked_Recv(SOCKET s, char *buf, int len, int flags) {
	CreateThreads();

	int ret = (*Original_Recv)(s, buf, len, flags);

	RecvFunction(buf, len);

	return ret;
}

int WINAPI Hooked_Send(SOCKET s, const char *buf, int len, int flags) {
	std::lock_guard<std::mutex> lock(Sender::Get()->acquire());

	CreateThreads();

	int ret = SendFunction(s, buf, len, flags, NULL, NULL);
	
	if (ret == 0) {
		ret = (*Original_Send)(s, buf, len, flags);
	}

	return ret;
}

void WINAPI Hooked_ExitProcess(UINT uExitCode) {
	Exit();
	Original_ExitProcess(uExitCode);
}

void WINAPI Hooked_Outputdebug(LPCSTR lpOutputString) {
	CreateThreads();
	std::string texto = lpOutputString;
	if (texto.find("motion") == std::string::npos &&
		texto.find("ChatEditproc") == std::string::npos &&
		texto.find("Move (") == std::string::npos &&
		texto.find("Sync Item Create") == std::string::npos &&
		texto.find("Sync ") == std::string::npos &&
		texto.find(" Create") == std::string::npos &&
		texto.find("serial") == std::string::npos &&
		texto.find("Enemy ") == std::string::npos &&
		texto.find("Tick:") == std::string::npos)
		printf("DEBUG: %s\n", lpOutputString);
	Original_Outputdebug(lpOutputString);
}

void ReadDigimon(Packet& packet, DigimonModel& digimon, bool retire) {
	if (&packet == NULL || &digimon == NULL) return;

	int position = packet.Reader().Position();

	int id = 0;
	const char* rid = NULL;

	if (retire) {
		rid = packet.Reader().GetString(16);
		id = packet.Reader().GetInt();
	}
	else {
		id = packet.Reader().GetInt();
		rid = packet.Reader().GetString(16);
	}

	if (!std::strlen(rid)) {
		packet.Skip(212);
		return;
	}

	if (id) digimon.id = id;
	if (std::strlen(rid)) digimon.rid = rid;

	digimon.model = packet.Reader().GetInt();
	digimon.unlockedStages = packet.Reader().GetByte();
	digimon.name = packet.Reader().GetString(20);
	packet.Skip(11);
	digimon.level = packet.Reader().GetShort();
	packet.Skip(2);
	digimon.str = packet.Reader().GetInt();
	digimon.dex = packet.Reader().GetInt();
	digimon.con = packet.Reader().GetInt();
	digimon.inte = packet.Reader().GetInt();
	for (int i = 0; i < (int)std::size(digimon.traits); i++) {
		digimon.traits[i] = packet.Reader().GetByte();
	}
	//Owner bonus
	//Stats STR,DEX,CON,INT
	packet.Skip(4);

	packet.Skip(4);

	//Stats ATT,DEF,BL
	packet.Skip(3);

	packet.Skip(9);

	digimon.stage = packet.Reader().GetByte();
	digimon.attribute = packet.Reader().GetByte();

	packet.Skip(6);

	digimon.hp = packet.Reader().GetInt();
	digimon.vp = packet.Reader().GetInt();
	digimon.evp = packet.Reader().GetInt();
	digimon.att = packet.Reader().GetInt();
	digimon.def = packet.Reader().GetInt();
	digimon.battlelevel = packet.Reader().GetInt();

	packet.Skip(4);

	digimon.exp = packet.Reader().GetInt();
	digimon.mtp = packet.Reader().GetInt();
	digimon.mhp = packet.Reader().GetInt();
	digimon.mvp = packet.Reader().GetInt();
	digimon.mevp = packet.Reader().GetInt();
	digimon.mexp = packet.Reader().GetInt();

	digimon.wins = packet.Reader().GetInt();
	digimon.losses = packet.Reader().GetInt();
	digimon.losses = digimon.losses - digimon.wins;

	digimon.statpoints = packet.Reader().GetShort();
	digimon.skillpoints = packet.Reader().GetShort();

	packet.Skip(4);

	for (int i = 0; i < (int)std::size(digimon.skills); i++) {
		digimon.skills[i].id = packet.Reader().GetInt();
		digimon.skills[i].levelreq = packet.Reader().GetInt();
		digimon.skills[i].at = packet.Reader().GetByte();
		digimon.skills[i].range = packet.Reader().GetByte();
		packet.Skip(6);
		digimon.skills[i].level = packet.Reader().GetInt();
	}

	packet.Skip(9);

	digimon.mutant = packet.Reader().GetByte();
	packet.Skip(2);
	digimon.mutantTime = packet.Reader().GetInt();
}

unsigned int getUnixTime() {
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	timeinfo->tm_hour += 5;
	return (unsigned int)mktime(timeinfo);
}

double distance(int x1, int y1, int x2, int y2)
{
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) * 1.0);
}

int randNumber(int min, int max) {
	return (int)min + (rand() % ((max + 1) - min));
}

double randDecimal(double min, double max) {
	return (double)min + (rand() / (RAND_MAX / ((max + 1) - min)));
}

void ShowPacket(const char* buf, int len) {
	int count = 0;
	for (int i = 0; i < len; i++) {
		if (count >= 16 && i + 1 < len) {
			for (int j = i - count; j < i; j++) {
				char c = buf[j];
				if (c <= 32 || c >= 126) c = '.';
				printf("%c", c);
			}
			count = 0;
			printf("\n");
		}
		printf("%.2X ", (uint8_t)buf[i]);
		count++;
	}
	printf("\n\n");
}

int SendFunction(SOCKET socket, const char* buf, int len, int flags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	if (packetSend) {
		if (packetSend->GetTotalLength() != packetSend->GetLength()) {
			packetSend->Writer().AddBytes(buf, len);
		}
		else packetSend->Reset(buf, len);
	}
	else packetSend = new Packet(buf, len);

	if (packetSend) {
		if (packetSend->GetTotalLength() == packetSend->GetLength()) {

			if (showSend &&
				!(std::find(std::begin(sendIgnore), std::end(sendIgnore), packetSend->GetType()) != std::end(sendIgnore)))
			{
				printf("SENDING Packet: %d LENGHT: %d(%d)\n",
					packetSend->GetType(),
					packetSend->GetTotalLength(),
					packetSend->GetLength()
				);

				ShowPacket(packetSend->Data(), packetSend->GetLength());
			}

			for (Module* module : _modules) {
				module->onSend(*packetSend);
			}
			//packetSend->disponse();
			//delete packetSend;
			if (socket) {
				return (*Original_Send)(socket, packetSend->Data(), packetSend->GetTotalLength(), flags);
			}
		}
	}

	return 0;
}



void RecvFunction(const char* buf, int len)
{
	if (packetRecv) {
		if (packetRecv->GetTotalLength() != packetRecv->GetLength()) {
			packetRecv->Writer().AddBytes(buf, len);
		}
		else packetRecv->Reset(buf, len);
	}
	else packetRecv = new Packet(buf, len);

	//printf("ORIGINAL:\n");
	//ShowPacket(buf, len);
	//printf("Packet:\n");
	/*if (packetRecv->Data()[1] == 99) {
		ShowPacket(packetRecv->Data(), packetRecv->GetTotalLength());
		printf("len: %d, %d = %d\n", len, packetRecv->GetLength(), packetRecv->GetTotalLength());
	}*/

	if (packetRecv) {
		if (packetRecv->GetTotalLength() == packetRecv->GetLength()) {
			if (showRecv &&
				!(std::find(std::begin(recvIgnore), std::end(recvIgnore), packetRecv->GetType()) != std::end(recvIgnore))) {

				printf("RECEIVING Packet: %d LENGHT: %d(%d)\n",
					packetRecv->GetType(),
					packetRecv->GetTotalLength(),
					packetRecv->GetLength()
				);

				ShowPacket(packetRecv->Data(), packetRecv->GetLength());
			}

			for (Module* module : _modules) {
				module->onRecv(*packetRecv);
			}

			//packetRecv->disponse();
			//delete packetRecv;
		}
	}
}

void processInput() {
	std::cout << "Waiting for a command:\n";
	while (running) {
		std::string input;
		std::getline(std::cin, input);
		if (input == "exit") {
			Exit();
			return;
		}
		if (input == "resetplayer") {
			_player->enable();
			_player->setBusy(false);
			continue;
		}
		if (input == "test") {

			sub_5132E0();
			sub_5158C0(39);
			/*int* v30 = sub_423CC0();
			*(int*)(v30 + 505) = 1;
			v30[128] = 0;
			v30[127] = 0;
			//int* v31 = sub_419AC0();
			//sub_419C40(*(int**)v31);*/


			/*MovePlayer(50,50);

			DWORD p = *(DWORD*)(sub_487100() + 0x42F0);
			printf("%p\n", p);

			DWORD p3 = *(DWORD*)(sub_487100() + 0x4308);
			printf("%p\n", p);
			
			sub_437EE0();
			*/
			
			continue;
		}
		if (input == "cls") {
			printf("\033c");
		}
		if (input.find("stop") != std::string::npos) {
			_player->setCancelAction(true);
			_player->setBusy(false);
		}
		if (input.find("sendk") != std::string::npos) {
			std::string texto = input.substr(input.find(" ") + 1, input.rfind(" ") - input.find(" ") - 1);
			sendKey(texto.c_str()[0]);
		}
		if (_player->getCharacter().id) {
			if (input.find("chat") != std::string::npos) {
				std::string texto = input.substr(input.find(" ") + 1, input.rfind(" ") - input.find(" ") - 1);

				char content[130];
				char playerName[21];
				char buff[sizeof(playerName) + sizeof(content) + 8];
				short size = sizeof(buff);

				char header[] = {
					(char)0xCC ,(char)0x21 ,(char)0x00 ,(char)0x00 ,
					(char)size & 0xFF,(char)(size >> 8) & 0xFF,(char)0x00 ,(char)0x00
				};

				memset(content, 0, sizeof(content));
				memset(playerName, 0, sizeof(playerName));
				memset(buff, 0, sizeof(buff));

				sprintf(content, "%s", texto.c_str());
				sprintf(playerName, "%s", "P1P1");

				std::copy(header, header + sizeof(header), buff);
				std::copy(playerName, playerName + sizeof(playerName), buff + sizeof(header));
				std::copy(content, content + sizeof(content), buff + sizeof(header) + sizeof(playerName));

				Sender::Get()->Send(buff, size, SERVER_TYPE::MAP);
			}
			if (input.find("loc") != std::string::npos) {
				printf("X: %d Y: %d Z: %d\n", _player->getCharacter().pos.x, _player->getCharacter().pos.y, _player->getCharacter().pos.z);
			}
			if (input.find("isbusy") != std::string::npos) {
				printf("Busy: %d\n", _player->isBusy());
			}
			if (input.find("move") != std::string::npos) {
				
				std::string xx = input.substr(input.find(" ") + 1, input.rfind(" ") - input.find(" ") - 1);
				std::string yy = input.substr(input.find(xx) + xx.length() + 1, input.length() - 1);

				short x = (short)std::stoi(xx);
				short y = (short)std::stoi(yy);

				if (x < 0) x = 0;
				if (y < 0) y = 0;

				if (_player->canMove(x, y)) {
					//printf("MANUAL MOVE: %d, %d\n", x, y);
					_player->setWalkTo(x, y, 0);
				}
				else {
					printf("Invalid position to move!\n");
				}
			}
		}
	}
}

void BotLoop() {
	while (running) {
		for (Module* module : _modules)
		{
			if (module->isEnabled())
			{
				module->update();
			}
		}
		sleep(250);
	}
}

void CreateConsole()
{
	if (!AllocConsole()) {
		//char buff[40];
		//sprintf(buff, "Alloc Console return error: %d", GetLastError());
		//MessageBox(NULL, buff, NULL, MB_ICONEXCLAMATION);
		return;
	}

	// std::cout, std::clog, std::cerr, std::cin
	freopen_s(&fDummy, "CONOUT$", "w", stdout);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONIN$", "r", stdin);

	std::cout.clear();
	std::clog.clear();
	std::cerr.clear();
	std::cin.clear();

	// std::wcout, std::wclog, std::wcerr, std::wcin
	HANDLE hConOut = CreateFile(("CONOUT$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hConIn = CreateFile(("CONIN$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
	SetStdHandle(STD_ERROR_HANDLE, hConOut);
	SetStdHandle(STD_INPUT_HANDLE, hConIn);

	std::wcout.clear();
	std::wclog.clear();
	std::wcerr.clear();
	std::wcin.clear();
}

void Hook() {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID &)Original_Send, Hooked_Send);
	DetourAttach(&(PVOID &)Original_Recv, Hooked_Recv);
	DetourAttach(&(PVOID &)Original_WSASend, Hooked_WSASend);
	DetourAttach(&(PVOID &)Original_WSARecv, Hooked_WSARecv);
	DetourAttach(&(PVOID &)Original_Connect, Hooked_Connect);
	//DetourAttach(&(PVOID &)Original_Bind, Hooked_Bind);
	//DetourAttach(&(PVOID &)Original_ExitProcess, Hooked_ExitProcess);
	DetourAttach(&(PVOID &)Original_Outputdebug, Hooked_Outputdebug);
	DetourTransactionCommit();
}

void UnHook() {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&(PVOID &)Original_Send, Hooked_Send);
	DetourDetach(&(PVOID &)Original_Recv, Hooked_Recv);
	DetourDetach(&(PVOID &)Original_WSASend, Hooked_WSASend);
	DetourDetach(&(PVOID &)Original_WSARecv, Hooked_WSARecv);
	DetourDetach(&(PVOID &)Original_Connect, Hooked_Connect);
	//DetourDetach(&(PVOID &)Original_Bind, Hooked_Bind);
	//DetourDetach(&(PVOID &)Original_ExitProcess, Hooked_ExitProcess);
	DetourDetach(&(PVOID &)Original_Outputdebug, Hooked_Outputdebug);
	DetourTransactionCommit();
}

std::vector<std::string> get_all_files_names_within_folder(std::string folder)
{
	std::vector<std::string> names;
	std::string search_path = folder + "/*.*";
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				names.push_back(fd.cFileName);
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
	return names;
}

std::string GetDirectory(std::string filename) {
	std::string directory;
	const size_t last_slash_idx = filename.rfind('\\');
	if (std::string::npos != last_slash_idx)
	{
		directory = filename.substr(0, last_slash_idx);
	}
	return directory;
}

void ReadMap(const char* file,const char* filepath) {
	std::string line;
	std::ifstream myfile (filepath);
	if (myfile.is_open())
	{
		int width, height;

		std::string temp;
		std::getline(myfile, temp);
		myfile >> temp;
		myfile >> height;
		myfile >> temp; myfile >> width;
		myfile >> temp;

		std::getline(myfile, temp);

		std::vector<unsigned char> passable{ '.', 'P' };
		unsigned char* pMap = new unsigned char[height*width]();

		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				char c; myfile >> c;
				pMap[width*j + i] = !(count(passable.begin(), passable.end(), c) > 0);
			}
		}

		myfile.close();
		temp.clear();

		for (int i = 0; i < (int)std::strlen(file); ++i) {
			if (file[i] >= '0' && file[i] <= '9') 
				temp = temp + file[i];
		}

		int id = std::stoi(temp.c_str());

		_player->getPathfinder().addMap(id, width, height, pMap);
	}
}

void Init() {
	srand((unsigned)time(NULL));

	CreateConsole();

	ZeroMemory(&Overlapped, sizeof(WSAOVERLAPPED));

	_player = new PlayerModule();
	_loot = new LootModule(_player);
	_move = new MoveModule(_player);
	_attack = new AttackModule(_player);
	_modules = { _player, _loot, _move, _attack };

	mainHandler2 = FindWindow(NULL, "DigimonBattle");
	mainHandler = FindWindow("DigimonBattle", "DigimonBattle");
	if (mainHandler != NULL) editHandler = FindWindowEx(mainHandler, NULL, "Edit", NULL);

	std::string path = getCurrentDllPath() +"\\Maps";
	std::vector<std::string> files = get_all_files_names_within_folder(path);

	//printf("Total files: %d\n", (int)files.size());

	for (auto&& it : files)
	{
		
		std::string filepath (path + '\\' + it);
		ReadMap(it.c_str(),filepath.c_str());

		//printf("%s\n", filepath.c_str());
	}

	files.clear();

	/*FILE* p_File = fopen("E:\\Jogos\\Pastas\\DWO\\DigimonBattle\\Map\\R001.map", "rb");
	fseek(p_File, 0, SEEK_END);
	long p_Length = ftell(p_File);
	fseek(p_File, 0, SEEK_SET);

	unsigned char *fileData = (unsigned char*)malloc(p_Length);
	fread(fileData, p_Length, 1, p_File);
	fclose(p_File);

	MapModel *map = new MapModel();
	map->ReadData((void*)&fileData);

	delete[] fileData;

	_player->getPathfinder().setMap(1);

	_player->getPathfinder().addMap(1, map);*/

	Hook();
}

void CreateThreads() {
	if (!threadCreated) {
		

		threadCreated = true;
		running = true;

		std::cout << "Initializing thread" << std::endl;

		inputThread = std::thread(processInput);
		botThread = std::thread(BotLoop);
	}
}

void Exit() {

	if (running) {
		running = false;

		if (threadCreated) {
			inputThread.detach();
			botThread.detach();
		}

		for (auto&& it : _modules) {
			it->disable();
		}

		ShowWindow(GetConsoleWindow(), SW_HIDE);

		//Lag and sometimes crash the client when close
		//FreeConsole();
		//fclose(fDummy);
	}

	if (packetSend) packetSend->disponse();
	if (packetRecv) packetRecv->disponse();


	std::mutex exitMutex;
	std::lock_guard<std::mutex> lock(exitMutex);

	for (auto&& it : _modules) {
		try
		{
			it->onDisponse();
		}
		catch (const std::exception& ex) {
			printf("ERROR:%s", ex.what());
		}
		delete it;
	}

	_modules.clear();
	
	if (DllHandle) {
		UnHook();
		FreeLibrary(DllHandle);
	}
}

unsigned char* ReadFile(std::string filename) {
	FILE* file = fopen(filename.c_str(), "rb");
	if (file) {
		fseek(file, 0, SEEK_END);
		long length = ftell(file);
		fseek(file, 0, SEEK_SET);

		unsigned char *fileData = (unsigned char*)malloc(length);
		fread(fileData, length, 1, file);

		fclose(file);

		return fileData;
	}
	else
	{
		printf("FILE NOT FOUND: %s", filename.c_str());
	}

	return NULL;
}

std::string getCurrentPath() {
	TCHAR dllPath[MAX_PATH];
	GetModuleFileName(NULL, dllPath, MAX_PATH);
	return GetDirectory(dllPath);
}
std::string getCurrentDllPath() {
	TCHAR dllPath[MAX_PATH];
	GetModuleFileName(DllHandle, dllPath, MAX_PATH);
	return GetDirectory(dllPath);
}

int main() {
	Init();

	const unsigned char * p1 = ReadFile(getCurrentDllPath()+"\\p1.bin");
	const unsigned char * p2 = ReadFile(getCurrentDllPath()+"\\p2.bin");
	const unsigned char * p3 = ReadFile(getCurrentDllPath()+"\\p3.bin");
	const unsigned char * p4 = ReadFile(getCurrentDllPath()+"\\p4.bin");
	const unsigned char * p5 = ReadFile(getCurrentDllPath()+"\\p5.bin");
	const unsigned char * p6 = ReadFile(getCurrentDllPath()+"\\p6.bin");
	const unsigned char * p7 = ReadFile(getCurrentDllPath()+"\\p7.bin");
	const unsigned char * p8 = ReadFile(getCurrentDllPath()+"\\p8.bin");
	const unsigned char * p9 = ReadFile(getCurrentDllPath()+"\\p9.bin");

	//RecvFunction((char*)p1, *(int*)(p1 + 4));
	//RecvFunction((char*)p2, *(int*)(p2 + 4));
	//RecvFunction((char*)p3, *(int*)(p3 + 4));
	//RecvFunction((char*)p4, *(int*)(p4 + 4));
	//RecvFunction((char*)p5, *(int*)(p5 + 4));
	//RecvFunction((char*)p6, *(int*)(p6 + 4));
	RecvFunction((char*)p7, *(int*)(p7 + 4));
	RecvFunction((char*)p7, *(int*)(p7 + 4));
	RecvFunction((char*)p8, *(int*)(p8 + 4));
	RecvFunction((char*)p7, *(int*)(p7 + 4));
	RecvFunction((char*)p8, *(int*)(p8 + 4));
	RecvFunction((char*)p9, *(int*)(p9 + 4));
	/*std::string path = getCurrentPath() + "\\Map\\R001.map";
	unsigned char* fileData = ReadFile(path.c_str());

	MapModel *map = new MapModel();
	map->ReadData((void*)&fileData);

	delete[] fileData;

	
	
	_player->getPathfinder().setMap(1);

	_player->getPathfinder().addMap(1, map);

	//const clock_t begin_time = clock();
	int startX = 6;
	int startY = 117;
	int endX = 50;
	int endY = 50;

	auto paths = _player->getPathfinder().find(startX, startY, endX, endY);

	//double ticks = clock() - begin_time;
	//printf("time: %-3.3fms\n", double(ticks / CLOCKS_PER_SEC) * 1000);

	_player->getPathfinder().Log();

	/*
	std::ofstream fw(getCurrentPath() + "\\map.txt", std::ofstream::out);

	if (fw.is_open())
	{
		for (int i = 0; i < map->width * map->height; i++) {
			if (i > 1 && i % map->width == 0) fw << std::endl;
			int y = (i / map->width);
			int x = i - ( y * map->width);
			//printf("%d,%d\n", x, y);
			bool contains = std::find(paths.begin(), paths.end(), std::tuple<int, int>(x,y)) != paths.end();
			if (x == startX && y == startY) fw << 'S';
			else if (x == endX && y == endY) fw << 'F';
			else if (contains) fw << '#';
			else fw << (map->data[i] ? '.' : 'O');
		}

		fw.close();
	}*/

	//for (auto&& it : paths)
	//{
		//printf("%d, %d\n", std::get<0>(it), std::get<1>(it));
	//}

	Exit();

	return 0;
}

BOOL WINAPI DllMain(HINSTANCE dllHandle, DWORD dwReason, LPVOID) {
	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
		DllHandle = dllHandle;
		Init();
		break;
	case DLL_PROCESS_DETACH:
		Exit();
		break;
	}
	return true;
}