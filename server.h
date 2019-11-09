// server.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>

using std::string;

using Store = std::unordered_map<string, string>;

void read(HANDLE h, Store &store);
Action readHeader(HANDLE h);
string readData(HANDLE h);
void returnData(HANDLE h, string data);
