#include <fstream>
#include <iostream>
#include "FtpHandler.h"
#include "Helpers.h"
#include "PatcherDialog.h"
#include "MainWindow.h"
#include "Data.h"

void ProgressObserver::SetDialog(PatcherDialog* dialog)
{
	mDialog = dialog;
}

void ProgressObserver::SetFileSize(long size)
{
	mDialog->SetFileSize(size);
}

void ProgressObserver::SetStatus(string status)
{
	mDialog->AddText(status);
}

void ProgressObserver::OnBytesReceived(const TByteVector& vBuffer, long lReceivedBytes)
{
	mDialog->AddBytesReceived(lReceivedBytes);
}
    
void ProgressObserver::OnBytesSent(const TByteVector& vBuffer, long lSentBytes)
{
	mDialog->AddBytesSent(lSentBytes);
}

FtpHandler::FtpHandler()
{
	
}

FtpHandler::~FtpHandler()
{
	// Logout from the FTP server.
	mFtpClient.Logout();
}

bool FtpHandler::Connect()
{
	WORD wVersionRequested;
	WSADATA wsaData;

	// Startup the usage of sockets.
    wVersionRequested = MAKEWORD(2, 2);
    WSAStartup(wVersionRequested, &wsaData);

	// Load login credentials.
	LoadCredentials(CREDENTIALS_FILE);

	CLogonInfo logonInfo(mHost, 21, mUser, mPass);

	// Attempt to login.
	return mFtpClient.Login(logonInfo);
}

// Returns true if there exist a newer version on the FTP server.
bool FtpHandler::NewVersion()
{
	remove("tmp.txt");

	// Compare the version files.
	mFtpClient.DownloadFile(mWorkingDirectory + CREDENTIALS_FILE, "tmp.txt", CType::Image(), true); 

	Data ftpData("tmp.txt");
	Data userData(CREDENTIALS_FILE);

	remove("tmp.txt");

	return userData.version < ftpData.version || userData.version > 9999;
}
	
// Download all files in the remote directory.
void FtpHandler::DownloadAll(string remoteDirectory, string localDirectory)
{
	// Get directory listing.
	TSpFTPFileStatusVector list;
	mFtpClient.List(remoteDirectory, list, true);

	// Loop through and download all files.
	for(auto iter = list.begin() + 2; iter != list.end(); iter++) {
		TSpFTPFileStatus file = (*iter);

		// Download the file.
		string ftpFilename = remoteDirectory + file->Name();
		string localFilename = localDirectory + file->Name();
		remove(localFilename.c_str());
		if(!mFtpClient.DownloadFile(ftpFilename, localFilename, CType::Image(), true))
			MessageBox(0, "Error downloading file.", "Download error!", 0);
		else {
			mObserver->SetStatus("- " + file->Name() + "\n");
		}
	}
}

void FtpHandler::DownloadFile(string file, string dest)
{
	// Download the file.
	string ftpFilename = mWorkingDirectory + file;
	remove(dest.c_str());

	if(!mFtpClient.DownloadFile(ftpFilename, dest, CType::Image(), true))
		MessageBox(0, "Error downloading file.", "Download error!", 0);
	else {
		mObserver->SetStatus("- " + file + "\n");
	}
}

void FtpHandler::UploadFile(string dest, string file)
{
	mObserver->SetFileSize(FileSize(file));
	mObserver->SetStatus("Uploading: " + file);
		
	// Remove and then upload the file.
	mFtpClient.Delete(dest + file);
	mFtpClient.UploadFile(file, dest + file, false, CRepresentation(CType::Image()), true);
}

// Loads the login credentials from a text file.
void FtpHandler::LoadCredentials(string file)
{
	Data data(CREDENTIALS_FILE);
	mHost = data.hostname;
	mUser = data.username;
	mPass = data.password;
	mWorkingDirectory = data.directory;
}

int FtpHandler::GetVersion()
{
	remove("tmp.txt");

	// Download the info.txt file
	if(!mFtpClient.DownloadFile(mWorkingDirectory + CREDENTIALS_FILE, "tmp.txt", CType::Image(), true)) 
		return 0;

	// Read the version.
	Data data("tmp.txt");
	remove("tmp.txt");
	return data.version;
}
	
int FtpHandler::GetNumFiles()
{
	remove("tmp.txt");

	// Download the info.txt file.
	if(!mFtpClient.DownloadFile(mWorkingDirectory + CREDENTIALS_FILE, "tmp.txt", CType::Image(), true)) 
		return 0;

	// Read num files.
	Data data("tmp.txt");
	remove("tmp.txt");
	return data.files;
}
	
int FtpHandler::GetTotalSize()
{
	remove("tmp.txt");

	// Download the info.txt file.
	if(!mFtpClient.DownloadFile(mWorkingDirectory + CREDENTIALS_FILE, "tmp.txt", CType::Image(), true)) 
		return 0;
		
	// Read total size.
	Data data("tmp.txt");
	remove("tmp.txt");
	return data.size;
}

string FtpHandler::GetModifyDate()
{
	remove("tmp.txt");

	// Download the info.txt file.
	if(!mFtpClient.DownloadFile(mWorkingDirectory + CREDENTIALS_FILE, "tmp.txt", CType::Image(), true)) 
		return "never";

	// Read the modify date.
	Data data("tmp.txt");
	remove("tmp.txt");
	return data.modifyDate;
}

void FtpHandler::SetObserver(ProgressObserver* observer)
{
	mObserver = observer;
	mFtpClient.AttachObserver(mObserver);
}