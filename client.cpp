// client.cpp : Defines the entry point for the application.
//

#include <windows.h>
#include <sstream>
#include <string>
#include <vector>
#include "common.h"
#include "client.h"
#include "serialization.h"

using namespace std;

void sendHeader(HANDLE h, string key) {
	cout << "Sending header." << endl;

	auto headerData{ serialize(Action { Type::Send, key }) };
	DWORD bytesWritten{ 0 };
	const BOOL success = WriteFile(
		h,
		&headerData[0],
		headerData.size(),
		&bytesWritten,
		nullptr // not overlapped i.e. synchronous
	);

	if (!success) {
		throw GetLastError();
	}
	cout << "Sent header." << endl;
}

bool awaitSendSuccess(HANDLE h) {
	std::vector<char> buf(bufSize);

	DWORD bytesRead;
	const BOOL readSuccess = ReadFile(h, buf.data(), buf.size(), &bytesRead, nullptr); // Use ReadFileEx for async
	if (!readSuccess) {
		throw GetLastError();
	}

	return string{ buf.data(), bytesRead } == saveSuccessStatus;
}
