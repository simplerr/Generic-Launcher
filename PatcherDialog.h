#pragma once
#include <string>
#include "BaseDialog.h"

using namespace std;

class ProgressObserver;

class PatcherDialog : public BaseDialog
{
public:
	PatcherDialog();
	~PatcherDialog();

	void UpdateClient();
	void DownloadLatest();
	void ExtractArchive(string dest);
	void RunApp();
	void AddText(string text, COLORREF color = RGB(0, 0, 0));
	void AddBytesSent(long bytes);
	void AddBytesReceived(long bytes);
	void SetFileSize(long size);
	LRESULT MsgProc(UINT msg, WPARAM wParam, LPARAM lParam);

	// Static entry point fot the upload thread.
	static void UdateThreadEntryPoint(void* pThis);
	static void RunThreadEntryPoint(void* pThis);
private:
	ProgressObserver* mObserver;
};