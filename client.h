// client.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include "common.h"
#include "serialization.h"

using std::string;

void sendHeader(HANDLE h, string key);
bool awaitSendSuccess(HANDLE h);

template <class T>
void send(string key, T data) {
	auto h = CreateFile(
		pipeName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		nullptr,
		OPEN_EXISTING,
		0,
		nullptr
	);
	if (h == INVALID_HANDLE_VALUE) {
		throw "Couldn't connect.";
	}

	DWORD mode{ PIPE_READMODE_MESSAGE };
	auto modeSet = SetNamedPipeHandleState(
		h,    // pipe handle 
		&mode,  // new pipe mode 
		nullptr,     // don't set maximum bytes 
		nullptr);    // don't set maximum time
	if (!modeSet) {
		throw "Couldn't put pipe into message mode.";
	}

	sendHeader(h, key);
	sendData(h, data);
	const bool success = awaitSendSuccess(h);

	CloseHandle(h);
	cout << "Send " << (success ? "success" : "fail") << endl;
}

template<class T>
T get(string key) {
	cout << "Getting data for key " << key << endl;
	auto headerData{ serialize(Action { Type::Get, key }) };
	std::vector<char> buf(bufSize);
	DWORD bytesRead;
	const BOOL success = CallNamedPipe(
		pipeName,
		&headerData[0],
		headerData.size(),
		buf.data(),
		buf.size(),
		&bytesRead,
		NMPWAIT_WAIT_FOREVER
	);
	if (!success) {
		throw GetLastError();
	}
	cout << "Got data for key " << key << endl;
	auto data{ deserialize<T>(string{buf.data(), bytesRead}) };
	return data;
}

template<class T>
void sendData(HANDLE h, T data) {
	cout << "Sending data." << endl;
	auto writeStr{ serialize(data) };

	DWORD bytesWritten{ 0 };
	const BOOL success = WriteFile(
		h,
		&writeStr[0],
		writeStr.size(),
		&bytesWritten,
		nullptr
	);

	if (!success) {
		// throw GetLastError();
		cout << "0x" << hex << GetLastError() << endl;
	}
	cout << "Sent data." << endl;
}
