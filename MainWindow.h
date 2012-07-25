#pragma once
#include "Runnable.h"

class FtpPatcher;

class MainWindow : public Runnable
{
public:
	MainWindow(HINSTANCE hInstance, string caption, int width, int height);
	~MainWindow();

	bool LaunchApp(string name);
private:
	FtpPatcher* mFtpPatcher;
};