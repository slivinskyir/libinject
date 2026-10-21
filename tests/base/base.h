#pragma once

#include "inject.hpp"

#define RUN_TEST(function)                 \
  printf(#function " : ");                 \
  function();                              \
  printf(" OK\n");

// Declare inject functions
INJECT_DECLARE(bool, DownloadFile, (const char *Path, char *Content));
INJECT_DECLARE(bool, DownloadFileNamespace, (const char *Path, char *Content));
