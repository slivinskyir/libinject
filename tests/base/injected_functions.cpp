// Generated file. Do not edit manually.

#include "app.h"
#include "inject.hpp"
#include <cstring>
#include <iostream>

// Source: app.cpp
namespace _inject {
bool DownloadFile(const char *Path, char *Content)
{
	std::cout << "MAIN\n";
	return false;
}
}
// Source: app.cpp
namespace _inject {
namespace ns {
bool DownloadFileNamespace(const char *Path, char *Content)
	{
		std::cout << "DownloadFileNamespace_ORIG\n";
		return false;
	}
}
}
