#include <windows.h>
#include <sstream>
#include <string>
#include <vector>
#include "common.h"
#include "client.h"
#include "serialization.h"

using namespace std;

void sendHeader(HANDLE h, string key) {
	log("Sending header.");

	auto headerData{ serialize(Action { Type::Send, key }) };
	DWORD bytesWritten{ 0 };
	const bool success = WriteFile(
		h,
		&headerData[0],
		headerData.size(),
		&bytesWritten,
		nullptr // not overlapped i.e. synchronous
	);

	if (!success) {
		throw GetLastError();
	}
	log("Sent header.");
}

bool awaitSendSuccess(HANDLE h) {
	vector<char> buf(bufSize);

	DWORD bytesRead;
	const bool readSuccess = ReadFile(
		h,
		buf.data(),
		buf.size(),
		&bytesRead,
		nullptr
	);
	if (!readSuccess) {
		throw GetLastError();
	}

	return string{ buf.data(), bytesRead } == saveSuccessStatus;
}
