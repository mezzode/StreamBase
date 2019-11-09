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

class CustomClass {
public:
	int counterA;

	CustomClass(int a = 0, int b = 0): counterA{a}, counterB{b} {}

	void incrementA() {
		++counterA;
	}

	void incrementB() {
		++counterB;
	}

	int getA() {
		return counterA;
	}

	int getB() {
		return counterB;
	}

private:
	int counterB;

	template<class Archive>
	friend void serialize(Archive& archive, CustomClass& m);
	// external serialize function must be friend to serialize private attribute
};

template<class Archive>
void serialize(Archive& archive, CustomClass& m) {
	archive(m.counterA, m.counterB);
}


int main()
{
	cout << "I am the client." << endl;

	send("mykey", 42);
	auto data{ get<int>("mykey") };
	cout << data << endl;

	send("foo", string{ "bar" });
	cout << get<string>("foo") << endl;

	CustomClass custom{ 10, 20 };
	custom.incrementA();
	custom.incrementB();
	send("mycustomclass", custom);
	auto savedCustom{ get<CustomClass>("mycustomclass") };
	savedCustom.incrementA();
	savedCustom.incrementB();
	cout << savedCustom.getA() << endl << savedCustom.getB() << endl;
	// should output 12 and 22

	getchar(); // wait before closing
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
