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

int get(string key) {
	auto headerData{ serialize(Action { Get, key }) };
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

	int data;
	{
		std::istringstream in{ string{buf.data(), bytesRead} };
		cereal::BinaryInputArchive iarchive{ in };
		iarchive(data);
	}
	cout << data << endl;
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

	getchar(); // wait before closing
}

void sendHeader(HANDLE h, string key) {
	cout << "Sending header." << endl;

	auto headerData{ serialize(Action { Send, key }) };
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
	cout << "Sent header.";
}

void sendData(HANDLE h, int data) {
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
}

template<class T>
string serialize(T data) {
	std::ostringstream out;
	{
		// not using portable binary archive since client and server on same machine so same endianness
		cereal::BinaryOutputArchive oarchive{ out };
		oarchive(data);
	}
	// writeStr.c_str() returns a const char * so would need to copy it to get a non-const *
	return out.str();
}