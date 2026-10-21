#include "app.h"
#include "inject.hpp"

#include <cstring>
#include <iostream>

inject__ bool DownloadFile(const char *Path, char *Content)
{
	std::cout << "MAIN\n";
	return false;
}

namespace ns {
	inject__ bool DownloadFileNamespace(const char *Path, char *Content)
	{
		std::cout << "DownloadFileNamespace_ORIG\n";
		return false;
	}
}

file GetFile(const char *Path)
{
	file File = {};

	bool Downloaded = DownloadFile(Path, File.Content);
	if (Downloaded)
	{
		File.Exists = true;
		File.SizeBytes = strlen(File.Content);
	} 

	return File;
}

file GetFileNamespace(const char *Path)
{
	file File = {};

	bool Downloaded = ns::DownloadFileNamespace(Path, File.Content);
	if (Downloaded)
	{
		File.Exists = true;
		File.SizeBytes = strlen(File.Content);
	} 

	return File;
}
