#pragma once

#include <string>
#include "ftp/FTPClient.h"

using namespace nsFTP;
using namespace std;

class FtpPatcher
{
public:
	FtpPatcher();
	~FtpPatcher();

	bool NewVersion();
	void DownloadAll(string remoteDirectory, string localDirectory);
	void LoadCredentials(string file);
private:
	CFTPClient mFtpClient;
	string mHost, mUser, mPass;
};