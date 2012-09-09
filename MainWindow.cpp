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
#include "Data.h"
#include "Helpers.h"

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
	gFtpHandler = new FtpHandler();

	mPatcherDialog = NULL;
	mExiting = false;

	// Is there a new version on the FTP server?
	/*if(!gFtpHandler->NewVersion() && 0)
	{
		Data data(CREDENTIALS_FILE);
		LaunchApp(LOCAL_FOLDER + data.executable);
		PostQuitMessage(0);
		mExiting = true;
	}*/
}
	
MainWindow::~MainWindow()
{
	delete gFtpHandler;
}

void MainWindow::Init()
{
	// Try to connect.
	if(!gFtpHandler->Connect()) 
	{
		// Show the patcher dialog.
		mPatcherDialog = new PatcherDialog();
		mPatcherDialog->Init(ERROR_CONNECTING);
	}
	else
	{
		// Is there a new version?
		if(!gFtpHandler->NewVersion()) {
			Data data(CREDENTIALS_FILE);
			LaunchApp(LOCAL_FOLDER + data.executable);
			PostQuitMessage(0);
			return;
			
		}
		else {
			mPatcherDialog = new PatcherDialog();
			mPatcherDialog->Init(NEW_VERSION_AVAILABLE);	
		}
	}
}

LRESULT MainWindow::MsgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Let the current state handle the message.
	if(mPatcherDialog != NULL)
		mPatcherDialog->MsgProc(msg, wParam, lParam);

	// Default MsgProc.
	return Runnable::MsgProc(msg, wParam, lParam);
}