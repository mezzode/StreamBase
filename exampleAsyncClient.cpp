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
		send("mykeyasync", 101);
	}) };

	auto strSend{ std::async(policy, []() {
		send("she", string{ "ra" });
	}) };
	
	auto customSend{ std::async(policy, []() {
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
