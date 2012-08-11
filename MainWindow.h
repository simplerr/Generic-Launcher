#pragma once
#include "Runnable.h"

class FtpPatcher;
class PatcherDialog;

class MainWindow : public Runnable
{
public:
	MainWindow(HINSTANCE hInstance, string caption, int width, int height);
	~MainWindow();

	void Init();
	bool LaunchApp(string name);
	LRESULT MsgProc(UINT msg, WPARAM wParam, LPARAM lParam);
private:
	PatcherDialog* mPatcherDialog;
};