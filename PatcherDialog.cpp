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
	mObserver = new ProgressObserver();
	mObserver->SetDialog(this);
	gFtpHandler->SetObserver(mObserver);

	AddText("Starting Auto Patcher v1.0\nComparing versions...\n");

	// Start a new thread that downloads the new files.
	_beginthread(PatcherDialog::UdateThreadEntryPoint,0, this);
}
	
PatcherDialog::~PatcherDialog()
{

}

void PatcherDialog::UpdateClient()
{
	if(gFtpHandler->NewVersion()) {
		AddText("New version found!\n");
		AddText("Downloading...\n");
		Data data(CREDENTIALS_FILE);
		DownloadLatest(data.directory);
		AddText("Extracting archive...\n");
		ExtractArchive("");
		remove("data.zip");
		AddText("Latest version downloaded and ready!\n");

		string patchNotes = ReadFileContnet(PATCH_NOTES_FILE);
		AddText("\nPatch notes for version: " + patchNotes + "\n\n");

		// Enable the Run button.
		EnableWindow(GetDlgItem(GetHwnd(), IDC_RUN), TRUE);

		data.ReadInformation(CREDENTIALS_FILE);
		AddText("Press \"Run\" to start " + data.executable +" \n");
	}
	else {
		AddText("Latest version found!\n");
		Sleep(500);
		Data data(CREDENTIALS_FILE);
		LaunchApp(LOCAL_FOLDER + data.executable);
		EndDialog(GetHwnd(), 0);
		PostQuitMessage(0);
	}
}

void PatcherDialog::UdateThreadEntryPoint(void* pThis)
{
	PatcherDialog* dialog = (PatcherDialog*)pThis;
	dialog->UpdateClient();
}

void PatcherDialog::AddText(string text, COLORREF color)
{
	AddEditText(GetDlgItem(GetHwnd(), IDC_ACTION_LOG), text, color);
	Sleep(250);
}

LRESULT PatcherDialog::MsgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(msg == IDC_RUN) {
		Data data(CREDENTIALS_FILE);
		LaunchApp(LOCAL_FOLDER + data.executable); 
		EndDialog(GetHwnd(), 0);
		PostQuitMessage(0);	
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
}

void PatcherDialog::SetFileSize(long size)
{
	SendDlgItemMessage(GetHwnd(), IDC_PROGRESS, PBM_SETRANGE32 , 0, size);
	SendDlgItemMessage(GetHwnd(), IDC_PROGRESS, PBM_SETPOS, 0, 0);
}

void PatcherDialog::DownloadLatest(string folder)
{
	// Download the data.txt and data.zip files.
	gFtpHandler->DownloadFile(CREDENTIALS_FILE, CREDENTIALS_FILE);
	gFtpHandler->DownloadFile(PATCH_NOTES_FILE, PATCH_NOTES_FILE);
	gFtpHandler->DownloadFile("data.zip", "data.zip");
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