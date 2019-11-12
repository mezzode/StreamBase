#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>

using std::string;

struct Store {
	// stl allows multiple threads reading and one writing so using a shared mutex
	std::unordered_map<string, string> records;
	std::shared_mutex lock;
};

void read(HANDLE h, Store &store);
Action readHeader(HANDLE h);
string readData(HANDLE h);
void returnData(HANDLE h, string data);
void saveSuccess(HANDLE h);
