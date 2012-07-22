#include "MainWindow.h"

// Set the globals.
MainWindow*	gMainWindow		= 0;

//! The program starts here.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	//InitAllocCheck(ACOutput_XML); // ACOutput_XML

	// Enable run-time memory check for debug builds.
	#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif

	MainWindow window(hInstance, "Generic Launcher", 700, 500);
	gMainWindow = &window;

	return gMainWindow->run();
}
