// server.cpp : Defines the entry point for the application.
//

#include <windows.h>
#include <thread>
#include <sstream>
#include <unordered_map>
#include <string>
#include "common.h"
#include "server.h"
#include "serialization.h"

using namespace std;

int main()
{
	cout << "I am the server." << endl;

	// TODO: add mutex. stl allows multiple threads reading and one writing
	auto store = unordered_map<string, string>{};

	while (TRUE) {
		const DWORD pipeMode{ PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT };
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
			cout << "0x" << hex << GetLastError() << endl;
			throw GetLastError();
		}
		cout << "Connected." << endl;

		// create a separate thread so can handle clients asynchronously
		thread{[h, &store]() {
			read(h, store);
			const BOOL dSuccess = DisconnectNamedPipe(h);
			if (!dSuccess) {
				cout << "0x" << hex << GetLastError() << endl;
				throw GetLastError();
			}
			CloseHandle(h);
		}}.detach();
	}
}

void read(HANDLE h, Store &store) {
	const auto action{ readHeader(h) };
	switch (action.type) {
		case Type::Send: {
			cout << "Saving data for key " << action.key << endl;
			store[action.key] = readData(h);
			cout << "Saved data for key " << action.key << endl;
			saveSuccess(h);
			break;
		}
		case Type::Get: {
			cout << "Getting data for key " << action.key << endl;
			returnData(h, store.at(action.key));
			cout << "Got data for key " << action.key << endl;
			break;
		}
	}
}

void saveSuccess(HANDLE h) {
	DWORD bytesWritten{ 0 };
	const BOOL success = WriteFile(
		h,
		&saveSuccessStatus[0],
		saveSuccessStatus.size(),
		&bytesWritten,
		nullptr
	);
	FlushFileBuffers(h);
	if (!success) {
		cout << "0x" << hex << GetLastError() << endl;
		throw GetLastError();
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
	FlushFileBuffers(h);
	if (!success) {
		cout << "0x" << hex << GetLastError() << endl;
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
