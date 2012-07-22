#include "MainWindow.h"
#include "FtpPatcher.h"

// Set the globals.
Runnable*	gMainWindow		= 0;

//! The program starts here.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
	#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif

	MainWindow window(hInstance, "Generic Launcher", 700, 500);
	gMainWindow = &window;

	return gMainWindow->run();
}

MainWindow::MainWindow(HINSTANCE hInstance, string caption, int width, int height)
	: Runnable(hInstance, caption, width, height)
{
	mFtpPatcher = new FtpPatcher();

	if(mFtpPatcher->NewVersion())
		mFtpPatcher->DownloadAll("/simplers.org/data/", "data/");
}
	
MainWindow::~MainWindow()
{
	delete mFtpPatcher;
}