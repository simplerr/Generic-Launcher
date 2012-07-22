#include <fstream>
#include "FtpPatcher.h"

FtpPatcher::FtpPatcher()
{
	WORD wVersionRequested;
	WSADATA wsaData;

	// Startup the usage of sockets.
    wVersionRequested = MAKEWORD(2, 2);
    WSAStartup(wVersionRequested, &wsaData);

	// Load login credentials.
	LoadCredentials("credentials.txt");

	CLogonInfo logonInfo(mHost, 21, mUser, mPass);

	// Attempt to login.
	if(!mFtpClient.Login(logonInfo))
		MessageBox(0, "Can't connect to server.", "Connection error", 0);
}

FtpPatcher::~FtpPatcher()
{
	// Logout from the FTP server.
	mFtpClient.Logout();
}

// Returns true if there exist a newer version on the FTP server.
bool FtpPatcher::NewVersion()
{
	// Compare the version files.
	mFtpClient.DownloadFile("/simplers.org/data/version.txt", "data/ftp_version.txt", CType::Image(), true); 
	std::ifstream fin("data/ftp_version.txt");

	// Read the versions.
	int ftpVersion, clientVersion;
	fin >> ftpVersion;
	fin.close();

	// Remove temp file.
	remove("data/ftp_version.txt");

	fin.open("data/version.txt");
	fin >> clientVersion;
	fin.close();

	// Does the client have the latest version?
	return clientVersion < ftpVersion;
}
	
// Download all files in the remote directory.
void FtpPatcher::DownloadAll(string remoteDirectory, string localDirectory)
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
			MessageBox(0, "Error downloading file.", "Update error", 0);
	}
}

// Loads the login credentials from a text file.
void FtpPatcher::LoadCredentials(string file)
{
	std::ifstream fin(file);
	fin >> mHost >> mUser >> mPass;
	fin.close();
}