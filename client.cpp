// client.cpp : Defines the entry point for the application.
//

#include <windows.h>
#include <sstream>
#include "cereal/archives/binary.hpp" // assuming we can use appropriate 3rd party libs
#include "common.h"
#include "client.h"

using namespace std;

int main()
{
	cout << "I am the client." << endl;

	send("mykey", 42);
}

int read(HANDLE h) {
	std::vector<char> buf(bufSize);

	DWORD bytesRead;
	const BOOL readSuccess = ReadFile(h, buf.data(), buf.size(), &bytesRead, nullptr); // Use ReadFileEx for async
	if (!readSuccess) {
		return GetLastError();
	}

	int test;
	{
		std::istringstream in{ std::string{buf.data(), bytesRead} };
		cereal::BinaryInputArchive iarchive{ in };
		iarchive(test);
	}
	cout << test << endl;
	return test;
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

	getchar(); // wait before closing
}

void sendHeader(HANDLE h, string key) {
	cout << "Sending header." << endl;

	DWORD bytesWritten{ 0 };

	const BOOL success = WriteFile(
		h,
		&key[0],
		key.size(),
		&bytesWritten,
		nullptr // not overlapped i.e. synchronous
	);

	if (!success) {
		throw GetLastError();
	}
	cout << "Sent header.";
}

void sendData(HANDLE h, int data) {
	std::ostringstream out;

	{
		// not using portable binary archive since client and server on same machine so same endianness
		cereal::BinaryOutputArchive oarchive{ out };
		oarchive(data);
	}

	auto writeStr{ out.str() };
	// writeStr.c_str() returns a const char * so would need to copy it to get a non-const *

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
}