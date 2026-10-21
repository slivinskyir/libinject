#pragma once

struct file 
{
	bool Exists;
	int SizeBytes;
	char Content[256];
};

file GetFile(const char *Path);
file GetFileNamespace(const char *Path);