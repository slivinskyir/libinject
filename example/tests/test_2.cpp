#include <cassert>
#include <cstdio>
#include <cstring>

#include "app.h"
#include "base/base.h"

void Test_FileDownload_Error2()
{
	INJECT_SET_LOCAL(DownloadFile, [](const char *Path, char *Content) 
	{ 
		return false; 
	});

	file File = GetFile("/test.txt");
	assert(File.Exists == false);
	assert(File.SizeBytes == 0);
}

void Test_FileDownload_Success3()
{
	INJECT_SET_LOCAL(DownloadFile, [](const char *Path, char *Content) {
		printf("Here"); 
		strcpy(Content, "ABC");
		return true; 
	});

	file File = GetFile("/test.txt");
	assert(File.Exists == true);
	assert(File.SizeBytes == 3);
}
