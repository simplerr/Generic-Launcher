// Use Windows 7 looking controls.
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <Richedit.h>
#include <Commctrl.h>
#include "MainWindow.h"
#include "FtpPatcher.h"
#include "PatcherDialog.h"
#include "FtpHandler.h"
#include "resource.h"

// Set the globals.
Runnable*	gMainWindow		= 0;
FtpHandler* gFtpHandler		= 0;

using namespace std;

//! The program starts here.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	InitCommonControls();
	LoadLibrary("riched32.dll");

	// Enable run-time memory check for debug builds.
	#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif

	//AllocConsole();

	MainWindow window(hInstance, "Generic Launcher", 700, 500);
	gMainWindow = &window;

	window.Init();

	return gMainWindow->Run();
}

MainWindow::MainWindow(HINSTANCE hInstance, string caption, int width, int height)
	: Runnable(hInstance, caption, width, height)
{
	gFtpHandler = new FtpHandler("data");
	mPatcherDialog = NULL;

	/*cout << "Generic Launcher v1.0" << endl;
	cout << "Comparing versions..." << endl;

	if(mFtpPatcher->NewVersion()) {
		cout << "New version found!\nStarting downloading updates...\n" << endl;
		mFtpPatcher->DownloadAll("/simplers.org/data/", "data/");
		cout << "Latest version downloaded!" << endl;
		cout << "Starting application..." << endl;
	}
	else {
		cout << "Latest version already installed..." << endl;
		cout << "Starting application..." << endl;
	}
	
	if(!LaunchApp("F:\\Windows\\System32\\notepad.exe"))
		cout << "Error launching application!" << endl;

	FreeConsole();

	cin.get();*/
}
	
MainWindow::~MainWindow()
{
	delete gFtpHandler;
}

void MainWindow::Init()
{
	mPatcherDialog = new PatcherDialog();
}

// Starts the specified application.
bool MainWindow::LaunchApp(string name)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Create the process.
	return CreateProcess(name.c_str(), NULL, NULL, NULL, false, 0, NULL, NULL, &si, &pi);
}

LRESULT MainWindow::MsgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(msg == IDM_LOADED)
		mPatcherDialog->UpdateClient();

	// Let the current state handle the message.
	if(mPatcherDialog != NULL)
		mPatcherDialog->MsgProc(msg, wParam, lParam);

	// Default MsgProc.
	return Runnable::MsgProc(msg, wParam, lParam);
}