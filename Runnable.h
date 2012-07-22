#pragma once

#define WIN32_LEAN_AND_MEAN
#include "ftp\FTPClient.h"
#include <string>

using namespace std;
void showMsg(std::string msg);

class Runnable
{
public:
	Runnable(HINSTANCE hInstance, string caption, int width, int height);
	virtual ~Runnable();

	int run();
	virtual LRESULT msgProc(UINT msg, WPARAM wParam, LPARAM lParam);

	void switchScreenMode();

	HINSTANCE				getInstance();
	HWND					getHwnd();
private:
	bool initWindow();
private:
	string			mCaption;
	HINSTANCE		mhInstance;
	HWND			mhMainWindow;
	int				mWidth, mHeight;

};	// Class

// Global
extern Runnable* gMainWindow;