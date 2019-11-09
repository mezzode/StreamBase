// server.cpp : Defines the entry point for the application.
//

#include <windows.h>
#include <sstream>
#include "cereal/archives/binary.hpp"
#include "common.h"
#include "server.h"

using namespace std;

int main()
{
	cout << "I am the server." << endl;

	const DWORD pipeMode{ PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT };
	// don't use PIPE_NOWAIT for async, see https://docs.microsoft.com/en-us/windows/win32/ipc/synchronous-and-overlapped-input-and-output

	const HANDLE h{ CreateNamedPipe(
		pipeName,
		PIPE_ACCESS_DUPLEX,
		pipeMode,
		PIPE_UNLIMITED_INSTANCES,
		bufSize,
		bufSize,
		0,
		nullptr
	) };
	if (h == INVALID_HANDLE_VALUE) {
		throw GetLastError();
	}

	const BOOL success = ConnectNamedPipe(h, nullptr);
	if (!success) {
		throw GetLastError();
	}

	read(h);
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