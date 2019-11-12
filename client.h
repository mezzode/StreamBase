#pragma once

#include <iostream>
#include <string>
#include <vector>
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

			// server hasnt yet made another instance for additional clients
			// so wait then try again
			auto instanceReady{ WaitNamedPipe(pipeName, NMPWAIT_WAIT_FOREVER) };
			if (!instanceReady) {
				cout << "Couldn't connect: 0x" << hex << GetLastError() << endl;
				throw "Couldn't connect.";
			}
		}
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
	log(string{ "Send " } + (success ? "success" : "fail"));
}

template<class T>
T get(string key) {
	log("Getting data for key " + key);
	auto headerData{ serialize(Action { Type::Get, key }) };
	vector<char> buf(bufSize);
	DWORD bytesRead;
	const bool success = CallNamedPipe(
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
	log("Got data for key " + key);
	auto data{ deserialize<T>(string{buf.data(), bytesRead}) };
	return data;
}

template<class T>
void sendData(HANDLE h, T data) {
	log("Sending data");
	auto writeStr{ serialize(data) };

	DWORD bytesWritten{ 0 };
	const bool success = WriteFile(
		h,
		&writeStr[0],
		writeStr.size(),
		&bytesWritten,
		nullptr
	);

	if (!success) {
		cout << "0x" << hex << GetLastError() << endl;
		throw GetLastError();
	}
	log("Sent data");
}
