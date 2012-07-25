#include <iostream>
#include "MainWindow.h"
#include "FtpPatcher.h"

// Set the globals.
Runnable*	gMainWindow		= 0;

using namespace std;

//! The program starts here.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
	#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif

	AllocConsole();

	MainWindow window(hInstance, "Generic Launcher", 700, 500);
	gMainWindow = &window;

	return gMainWindow->run();
}

MainWindow::MainWindow(HINSTANCE hInstance, string caption, int width, int height)
	: Runnable(hInstance, caption, width, height)
{
	mFtpPatcher = new FtpPatcher();

	cout << "Generic Launcher v1.0" << endl;
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

	cin.get();
}
	
MainWindow::~MainWindow()
{
	delete mFtpPatcher;
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