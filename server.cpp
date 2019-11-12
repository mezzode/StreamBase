#include <windows.h>
#include <shared_mutex>
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
	log("I am the server");
	
	Store store;

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

		const bool success = ConnectNamedPipe(h, nullptr);
		if (!success) {
			cout << "0x" << hex << GetLastError() << endl;
			throw GetLastError();
		}
		log("Connected to a client");

		// create a separate thread so can handle clients asynchronously
		thread{[h, &store]() {
			read(h, store);
			FlushFileBuffers(h);
			const bool dSuccess = DisconnectNamedPipe(h);
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
			log("Saving data for key " + action.key);
			{
				// only one thread can write at a time
				lock_guard<shared_mutex> lg{ store.lock };
				store.records[action.key] = readData(h);
			}
			log("Saved data for key " + action.key);
			saveSuccess(h);
			break;
		}
		case Type::Get: {
			log("Getting data for key " + action.key);
			{
				// any number of threads can read
				shared_lock<shared_mutex> lg{ store.lock };
				returnData(h, store.records.at(action.key));
			}
			log("Got data for key " + action.key);
			break;
		}
	}
}

void saveSuccess(HANDLE h) {
	DWORD bytesWritten{ 0 };
	const bool success = WriteFile(
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
	const bool success = WriteFile(
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
	vector<char> buf(bufSize);

	DWORD bytesRead;
	const bool readSuccess = ReadFile(h, buf.data(), buf.size(), &bytesRead, nullptr);
	if (!readSuccess) {
		throw GetLastError();
	}

	return string{ buf.data(), bytesRead };
}

Action readHeader(HANDLE h) {
	vector<char> buf(bufSize);

	DWORD bytesRead;
	const bool readSuccess = ReadFile(h, buf.data(), buf.size(), &bytesRead, nullptr);
	if (!readSuccess) {
		throw GetLastError();
	}

	auto action = deserialize<Action>(string{ buf.data(), bytesRead });
	return action;
}
