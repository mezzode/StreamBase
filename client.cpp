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

	write("mykey", 42);
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

void write(string key, int data) {
	sendHeader(key);

	std::ostringstream out;

	{
		cereal::BinaryOutputArchive oarchive{ out };
		oarchive(data);
	}

	auto writeStr = out.str();
	// writeStr.c_str() returns a const char * so would need to copy it to get a non-const *

	// strings use contiguous array so this should work?

	// either open pipe once and reuse handle by doing CreateFile then TransactNamedPipe
	// or open and close pipe for each message by using CallNamedPipe

	// for now using CallNamedPipe for simplicity

	// set buffer size depending on size of data we're sending?

	// not using portable binary archive since client and server on same machine so same endianness

	const BOOL success = CallNamedPipe(
		pipeName,
		&writeStr[0],
		writeStr.size(),
		nullptr,
		0,
		nullptr,
		NMPWAIT_WAIT_FOREVER
	);

	if (!success) {
		// throw GetLastError();
		cout << "0x" << hex << GetLastError() << endl;
	}

	getchar(); // wait before closing
}

void sendHeader(string key) {
	cout << "Sending header." << endl;

	const BOOL success = CallNamedPipe(
		pipeName,
		&key[0],
		key.size(),
		nullptr,
		0,
		nullptr,
		NMPWAIT_WAIT_FOREVER
	);

	if (!success) {
		throw GetLastError();
	}
	cout << "Sent header.";
}