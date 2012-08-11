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

// Procedure for the dialog box.
LRESULT CALLBACK PatcherDlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_COMMAND:
		switch(wParam)
		{
		case IDOK:
			
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

	if(gFtpHandler->NewVersion()) {
		AddText("New version found!\n");
		AddText("Downloading...\n");
		Data data("data.txt");
		DownloadLatest(data.directory);
		AddText("Extracting archive...\n");
		ExtractArchive("data/");
		AddText("Latest version downloaded and ready!\n");
	}
	else {
		AddText("Latest version found!\n");
	}
}
	
PatcherDialog::~PatcherDialog()
{

}

void PatcherDialog::AddText(string text, COLORREF color)
{
	AddEditText(GetDlgItem(GetHwnd(), IDC_ACTION_LOG), text, color);
}

LRESULT PatcherDialog::MsgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

void PatcherDialog::AddBytesSent(long bytes)
{
	long pos = SendDlgItemMessage(GetHwnd(), IDC_PROGRESS1, PBM_GETPOS, 0, 0);
	SendDlgItemMessage(GetHwnd(), IDC_PROGRESS1, PBM_SETPOS, pos + bytes, 0);
}
	
void PatcherDialog::AddBytesReceived(long bytes)
{

}

void PatcherDialog::SetFileSize(long size)
{
	SendDlgItemMessage(GetHwnd(), IDC_PROGRESS1, PBM_SETRANGE32 , 0, size);
	SendDlgItemMessage(GetHwnd(), IDC_PROGRESS1, PBM_SETPOS, 0, 0);
}

void PatcherDialog::DownloadLatest(string folder)
{
	// Download the data.txt and data.zip files.
	gFtpHandler->DownloadFile("data.txt", "data.txt");
	gFtpHandler->DownloadFile("data.zip", "data/data.zip");
}

void PatcherDialog::ExtractArchive(string dest)
{

}