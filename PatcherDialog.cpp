#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Richedit.h>
#include <Commctrl.h>
#include "PatcherDialog.h"
#include "MainWindow.h"
#include "resource.h"
#include "Helpers.h"
#include "FtpHandler.h"
#include "Data.h"
#include "unzip.h"

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

	AddText("Starting Auto Patcher v1.0...\n");
	AddText("Comparing versions...\n");

	UpdateClient();
}
	
PatcherDialog::~PatcherDialog()
{

}

void PatcherDialog::UpdateClient()
{
	if(gFtpHandler->NewVersion()) {
		AddText("New version found!\n");
		AddText("Downloading...\n");
		Data data("data.txt");
		DownloadLatest(data.directory);
		AddText("Extracting archive...\n");
		ExtractArchive("");
		remove("data.zip");
		AddText("Latest version downloaded and ready!\n");

		// Enable the Run button.
		EnableWindow(GetDlgItem(GetHwnd(), IDC_RUN), TRUE);

		data.ReadInformation("data.txt");
		AddText("Press \"Run\" to start " + data.executable +" \n");
	}
	else {
		AddText("Latest version found!\n");
		//Sleep(1000);
		Data data("data.txt");
		LaunchApp("data/" + data.executable); // [NOTE]  "data/"
		EndDialog(GetHwnd(), 0);
		PostQuitMessage(0);
	}
}

bool PatcherDialog::LaunchApp(string app)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Create the process.
	return CreateProcess(app.c_str(), NULL, NULL, NULL, false, 0, NULL, NULL, &si, &pi);
}

void PatcherDialog::AddText(string text, COLORREF color)
{
	AddEditText(GetDlgItem(GetHwnd(), IDC_ACTION_LOG), text, color);
	Sleep(250);
}

LRESULT PatcherDialog::MsgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(msg == IDM_LOADED)
		int asda = 1;
	else if(msg == IDC_RUN) {
		Data data("data.txt");
		LaunchApp("data/" + data.executable); 
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
	gFtpHandler->DownloadFile("data.txt", "data.txt");
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