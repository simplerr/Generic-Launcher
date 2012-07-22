#pragma once

#include <Windows.h>
#include <string>

using namespace std;
void showMsg(std::string msg);

class MainWindow
{
public:
	MainWindow(HINSTANCE hInstance, std::string caption, int width, int height);
	virtual ~MainWindow();

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
extern MainWindow* gMainWindow;