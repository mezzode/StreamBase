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
	HANDLE h{ INVALID_HANDLE_VALUE };
	while (h == INVALID_HANDLE_VALUE) {
		h = CreateFile(
			pipeName,
			GENERIC_READ | GENERIC_WRITE,
			0,
			nullptr,
			OPEN_EXISTING,
			0,
			nullptr
		);

		if (h == INVALID_HANDLE_VALUE) {
			auto e = GetLastError();
			if (e != ERROR_PIPE_BUSY) {
				cout << "0x" << hex << GetLastError() << endl;
				throw "Couldn't connect.";
			}

			// else server hasnt yet made another instance for additional clients so wait
			auto instanceReady{ WaitNamedPipe(pipeName, NMPWAIT_WAIT_FOREVER) };
			if (!instanceReady) {
				cout << "Couldn't connect: 0x" << hex << GetLastError() << endl;
				throw "Couldn't connect.";
			}
		}
	}

	cout << "CreateFile " << key << endl;

	DWORD mode{ PIPE_READMODE_MESSAGE };
	auto modeSet = SetNamedPipeHandleState(
		h,    // pipe handle 
		&mode,  // new pipe mode 
		nullptr,     // don't set maximum bytes 
		nullptr);    // don't set maximum time
	if (!modeSet) {
		throw "Couldn't put pipe into message mode.";
	}

	cout << "Mode set " << key << endl;

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
