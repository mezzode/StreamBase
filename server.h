// server.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <string>
#include <utility>

using std::string;

std::pair<string, string> read(HANDLE h);
string readHeader(HANDLE h);
string readData(HANDLE h);