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

	auto store = unordered_map<string, string>{};

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

	cout << "Connected to client." << endl;

	while (TRUE) {
		read(h, store);
	}
}

void read(HANDLE h, Store &store) {
	const auto action{ readHeader(h) };
	switch (action.type) {
		case Send: {
			store[action.key] = readData(h);
		}
		case Get: {
			returnData(h, store.at(action.key));
		}
	}
}

void returnData(HANDLE h, string data) {
	DWORD bytesWritten{ 0 };
	const BOOL success = WriteFile(
		h,
		&data[0],
		data.size(),
		&bytesWritten,
		nullptr
	);
	if (!success) {
		throw GetLastError();
	}
}

string readData(HANDLE h) {
	std::vector<char> buf(bufSize);

	DWORD bytesRead;
	const BOOL readSuccess = ReadFile(h, buf.data(), buf.size(), &bytesRead, nullptr); // Use ReadFileEx for async
	if (!readSuccess) {
		throw GetLastError();
	}

	return string{ buf.data(), bytesRead };
}

Action readHeader(HANDLE h) {
	std::vector<char> buf(bufSize);

	DWORD bytesRead;
	const BOOL readSuccess = ReadFile(h, buf.data(), buf.size(), &bytesRead, nullptr); // Use ReadFileEx for async
	if (!readSuccess) {
		throw GetLastError();
	}

	auto action = deserialize<Action>(string{ buf.data(), bytesRead });
	return action;
}

template<class T>
T deserialize(const string &serializedData) {
	T data;
	{
		std::istringstream in{ serializedData };
		cereal::BinaryInputArchive iarchive{ in };
		iarchive(data);
	}
	// writeStr.c_str() returns a const char * so would need to copy it to get a non-const *
	return data;
}