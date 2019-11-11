#include "client.h"
#include "exampleClient.h"

using namespace std;

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

CustomClass::CustomClass(int a = 0, int b = 0) : counterA{ a }, counterB{ b } {}

void CustomClass::incrementA() {
	++counterA;
}

void CustomClass::incrementB() {
	++counterB;
}

int CustomClass::getA() {
	return counterA;
}

int CustomClass::getB() {
	return counterB;
}

template<class Archive>
void serialize(Archive& archive, CustomClass& m) {
	archive(m.counterA, m.counterB);
}
