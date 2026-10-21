#include <cassert>
#include <cstdio>
#include <cstring>

#include "app.h"
#include "base/base.h"

// Having them in a separate file for testing purpose (ODR valiolation check)
extern void Test_FileDownload_Error2();
extern void Test_FileDownload_Success3();

void Test_FileDownload_Error()
{
	INJECT_SET_LOCAL(DownloadFile, [](const char *Path, char *Content) 
	{ 
		return false; 
	});

	file File = GetFile("/test.txt");
	assert(File.Exists == false);
	assert(File.SizeBytes == 0);
}

void Test_FileDownload_Success()
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

void Test_DownloadFileNamespace_Error()
{
	INJECT_SET_LOCAL(DownloadFileNamespace, [](const char *Path, char *Content) 
	{ 
		return false; 
	});

	file File = GetFileNamespace("/test.txt");
	assert(File.Exists == false);
	assert(File.SizeBytes == 0);
}

void Test_DownloadFileNamespace_Success()
{
	INJECT_SET_LOCAL(DownloadFileNamespace, [](const char *Path, char *Content) {
		printf("Here"); 
		strcpy(Content, "ABC");
		return true; 
	});

	file File = GetFileNamespace("/test.txt");
	assert(File.Exists == true);
	assert(File.SizeBytes == 3);
}

int main()
{
	RUN_TEST(Test_FileDownload_Error);
	RUN_TEST(Test_FileDownload_Success);

	RUN_TEST(Test_FileDownload_Error2);
	RUN_TEST(Test_FileDownload_Success3);

	RUN_TEST(Test_DownloadFileNamespace_Error);
	RUN_TEST(Test_DownloadFileNamespace_Success);

	return 0;
}