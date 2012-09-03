#pragma once

#define WIN32_LEAN_AND_MEAN
#include "ftp\FTPClient.h"
#include <string>

using namespace std;
void showMsg(std::string msg);

//! Inits the window and contains the main loop.
class Runnable
{
public:
	Runnable(HINSTANCE hInstance, string caption, int width, int height);
	virtual ~Runnable();

	int Run();
	virtual void Init() {};
	virtual LRESULT MsgProc(UINT msg, WPARAM wParam, LPARAM lParam);
	void Kill();

	void SetVisible(bool visible);
	void SwitchScreenMode();

	bool					InitWindow();
	HINSTANCE				GetInstance();
	HWND					GetHwnd();
private:
	string			mCaption;
	HINSTANCE		mhInstance;
	HWND			mhMainWindow;
	int				mWidth, mHeight;
	bool			mDead;
};	// Class

// Global
extern Runnable* gMainWindow;