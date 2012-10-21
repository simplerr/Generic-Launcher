#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Richedit.h>
#include <fstream>
#include <Commctrl.h>
#include <process.h>
#include "PatcherDialog.h"
#include "MainWindow.h"
#include "resource.h"
#include "Helpers.h"
#include "FtpHandler.h"
#include "Data.h"
#include "unzip.h"

using namespace std;

bool init = false;

// Procedure for the dialog box.
LRESULT CALLBACK PatcherDlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG: 
		{
		int asda = 1;
		break;
		}
	case WM_COMMAND:
		switch(wParam)
		{
		case IDC_RUN:
			SendMessage(gMainWindow->GetHwnd(), IDC_RUN, 0, 0);
			break;
		case IDC_DOWNLOAD:
			SendMessage(gMainWindow->GetHwnd(), IDC_DOWNLOAD, 0, 0);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hWndDlg, 0);
		PostQuitMessage(0);
		break;
	}

	return 0;
}

PatcherDialog::PatcherDialog()
	: BaseDialog(IDD_DIALOG1, (DLGPROC)PatcherDlgProc)
{
	
}
	
PatcherDialog::~PatcherDialog()
{

}

void PatcherDialog::Init(InitState initState)
{
	if(initState == ERROR_CONNECTING) 
	{
		AddText("Error connecting to the server!\n");
		AddText("Press the Run button to start your current version\n");
		EnableWindow(GetDlgItem(GetHwnd(), IDC_RUN), true); 
	}
	else if(initState == NEW_VERSION_AVAILABLE)
	{
		AddText("New version found!\n");
		AddText("Download or run current version?\n");

		mObserver = new ProgressObserver();
		mObserver->SetDialog(this);
		gFtpHandler->SetObserver(mObserver);

		// Show the skip and download buttons.
		EnableWindow(GetDlgItem(GetHwnd(), IDC_DOWNLOAD), true);
		EnableWindow(GetDlgItem(GetHwnd(), IDC_RUN), true);
		UpdateWindow(GetHwnd());
	}
}

void PatcherDialog::UdateThreadEntryPoint(void* pThis)
{
	PatcherDialog* dialog = (PatcherDialog*)pThis;
	dialog->DownloadLatest();
}

void PatcherDialog::RunThreadEntryPoint(void* pThis)
{
	PatcherDialog* dialog = (PatcherDialog*)pThis;
	dialog->RunApp();
}

void PatcherDialog::AddText(string text, COLORREF color)
{
	AddEditText(GetDlgItem(GetHwnd(), IDC_ACTION_LOG), text, color);
}

void PatcherDialog::RunApp()
{
	Data data(CREDENTIALS_FILE);
	AddText("Starting " + data.executable + "...\n");
	Sleep(1000.0f);
	LaunchApp(LOCAL_FOLDER + data.executable); 
	EndDialog(GetHwnd(), 0);
	PostQuitMessage(0);
	gMainWindow->Kill();
	Sleep(500.0f);
}

LRESULT PatcherDialog::MsgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(msg == IDC_RUN) {
		// Disable the buttons.
		EnableWindow(GetDlgItem(GetHwnd(), IDC_DOWNLOAD), false);
		EnableWindow(GetDlgItem(GetHwnd(), IDC_RUN), false);

		// New thread for starting the app.
		_beginthread(PatcherDialog::RunThreadEntryPoint, 0, this);
	}
	else if(msg == IDC_DOWNLOAD) {
		// Hide the skip and download buttons.
		ShowWindow(GetDlgItem(GetHwnd(), IDC_DOWNLOAD), false);
		EnableWindow(GetDlgItem(GetHwnd(), IDC_RUN), false);
		UpdateWindow(GetDlgItem(GetHwnd(), IDC_RUN));

		SendDlgItemMessage(GetHwnd(), IDC_PROGRESS, PBM_SETPOS, 0, 0);

		// New thread for downloading the files.
		_beginthread(PatcherDialog::UdateThreadEntryPoint, 0, this);
	}

	return 0;
}

void PatcherDialog::AddBytesSent(long bytes)
{
	long pos = SendDlgItemMessage(GetHwnd(), IDC_PROGRESS, PBM_GETPOS, 0, 0);
	SendDlgItemMessage(GetHwnd(), IDC_PROGRESS, PBM_SETPOS, pos + bytes, 0);
}
	
void PatcherDialog::AddBytesReceived(long bytes)
{
	long pos = SendDlgItemMessage(GetHwnd(), IDC_PROGRESS, PBM_GETPOS, 0, 0);
	SendDlgItemMessage(GetHwnd(), IDC_PROGRESS, PBM_SETPOS, pos + bytes, 0);
	mBytesReceived += bytes;
	char buffer[256];
	sprintf(buffer, "%.1f/%.1fmb", (float)mBytesReceived/1000000, (float)mFileSize/1000000);
	SetWindowText(GetDlgItem(GetHwnd(), IDC_DOWNLOADED), buffer); 
}

void PatcherDialog::SetFileSize(long size)
{
	SendDlgItemMessage(GetHwnd(), IDC_PROGRESS, PBM_SETRANGE32 , 0, size);
	SendDlgItemMessage(GetHwnd(), IDC_PROGRESS, PBM_SETPOS, 0, 0);
	mFileSize = size;
	mBytesReceived = 0;
}

void PatcherDialog::DownloadLatest()
{
	AddText("Downloading...\n");
	Data data(CREDENTIALS_FILE);

	// Download the data.txt and data.zip files.
	gFtpHandler->DownloadFile(CREDENTIALS_FILE, CREDENTIALS_FILE);
	gFtpHandler->DownloadFile(PATCH_NOTES_FILE, PATCH_NOTES_FILE);
	gFtpHandler->DownloadFile("data.zip", "data.zip");

	AddText("Extracting archive...\n");
	ExtractArchive("");
	remove("data.zip");
	AddText("Latest version downloaded and ready!\n");

	string patchNotes = ReadFileContnet(PATCH_NOTES_FILE);
	AddText("Patch notes for version: " + patchNotes + "\n\n");

	// Enable the Run button.
	EnableWindow(GetDlgItem(GetHwnd(), IDC_RUN), true);

	data.ReadInformation(CREDENTIALS_FILE);
	AddText("Press Run to start " + data.executable + "\n");
}

void PatcherDialog::ExtractArchive(string dest)
{
	HZIP hz = OpenZip("data.zip", 0);
	ZIPENTRY ze;
	GetZipItem(hz, -1, &ze);
	int items = ze.index;

	for(int zi = 0; zi < items; zi++)
	{
		ZIPENTRY ze;
		GetZipItem(hz, zi, &ze);
		string dst = dest + ze.name;
		UnzipItem(hz, zi, dst.c_str());
	}

	CloseZip(hz);
}