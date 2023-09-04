#ifndef WIN_FUNCTIONS_H
#define WIN_FUNCTIONS_H

#include <windows.h>
#include <thread>

extern HWND mainHandler;
extern HWND mainHandler2;
extern HWND editHandler;

void sleep(int ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void leftClick(int x, int y) {
	if (mainHandler != NULL) {
		PostMessage(mainHandler, WM_LBUTTONDOWN, 1, MAKELPARAM(x, y));
		sleep(50);
		PostMessage(mainHandler, WM_LBUTTONUP, 0, MAKELPARAM(x, y));
		sleep(50);
	}
}

void rightClick(int x, int y) {
	if (mainHandler != NULL) {
		PostMessage(mainHandler, WM_RBUTTONDOWN, 1, MAKELPARAM(x, y));
		sleep(50);
		PostMessage(mainHandler, WM_RBUTTONUP, 0, MAKELPARAM(x, y));
		sleep(50);
	}
}

void moveCursor(int x, int y) {
	if (mainHandler != NULL) {
		PostMessage(mainHandler, WM_MOUSEMOVE, 0, MAKELPARAM(x, y));
		sleep(50);
	}
}

void sendKey(char key) {
	if (editHandler != NULL) {
		PostMessage(editHandler, WM_SETFOCUS, 0, 0);
		PostMessage(editHandler, WM_KEYDOWN, VkKeyScan(key), 0x200001);
		sleep(50);
		//PostMessage(mainHandler, WM_SETFOCUS, 0, 0);
		PostMessage(mainHandler2, WM_KEYUP, VkKeyScan(key), 0xC0200001);
		sleep(50);
	}
}

void sendText(const char* text, int textSize) {
	if (editHandler != NULL) {
		SendMessage(editHandler, WM_SETTEXT, 0, (LPARAM)"");
		for (int i = 0; i < textSize; i++) {
			sendKey(text[i]);
		}
	}
}
#endif