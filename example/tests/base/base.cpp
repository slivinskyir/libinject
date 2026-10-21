#include "base.h"

INJECT_DEFINE(bool, DownloadFile, (const char *Path, char *Content), (Path, Content));
INJECT_DEFINE_NS(ns, bool, DownloadFileNamespace, (const char *Path, char *Content), (Path, Content));