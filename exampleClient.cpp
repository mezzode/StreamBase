#include "client.h"
#include "CustomClass.h"

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
