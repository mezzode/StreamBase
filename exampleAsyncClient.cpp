#include "client.h"
#include "CustomClass.h"

using namespace std;

#include <future>

int main()
{
	cout << "I am the async client." << endl;

	const auto policy{ std::launch::async };

	// sends

	auto intSend{ std::async(policy, []() {
		cout << "Sending int" << endl;
		send("mykeyasync", 101);
	}) };

	auto strSend{ std::async(policy, []() {
		cout << "Sending string" << endl;
		try {
			send("she", string{ "ra" });
		} catch (char e[]) {
			// manually catch errors since exceptions only propagate on .get()
			cout << e << endl;
			std::terminate();
		}
		cout << "Sent string" << endl;
	}) };
	
	auto customSend{ std::async(policy, []() {
		cout << "Sending custom class" << endl;
		CustomClass custom{ 42, 81 };
		custom.incrementA();
		custom.incrementB();
		send("mycustomclassasync", custom);
	}) };

	// gets

	auto intGet{ std::async(policy, [&intSend]() {
		intSend.get(); // await send success
		cout << get<int>("mykeyasync") << endl;
	}) };

	auto strGet{ std::async(policy, [&strSend]() {
		strSend.get(); // await send success
		cout << get<string>("she") << endl;
	}) };

	auto customGet{ std::async(policy, [&customSend]() {
		customSend.get(); // await send success
		auto savedCustom{ get<CustomClass>("mycustomclassasync") };
		savedCustom.incrementA();
		savedCustom.incrementB();
		cout << savedCustom.getA() << endl << savedCustom.getB() << endl;
	}) };

	getchar(); // wait before closing
}
