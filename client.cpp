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

int main()
{
	cout << "I am the client." << endl;

	send("mykey", 42);

	auto data{ get("mykey") };
	cout << data << endl;

	getchar(); // wait before closing
}

int get(string key) {
	cout << "Getting data for key " << key << endl;
	auto headerData{ serialize(Action { Type::Get, key }) };
	vector<char> buf(bufSize);
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
	auto data{ deserialize<int>(string{buf.data(), bytesRead}) };
	return data;
}

void send(string key, int data) {
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
	sendData(h, 42);

	CloseHandle(h);
	cout << "Send success." << endl;
}

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

void sendData(HANDLE h, int data) {
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
