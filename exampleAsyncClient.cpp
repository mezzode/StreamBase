#include "client.h"
#include "CustomClass.h"

using namespace std;

#include <future>

int main()
{
	log("I am the async client.");

	const auto policy{ launch::async };

	const string intKey{ "MyKeyAsync" };
	const string strKey{ "She" };
	const string customKey{ "CustomClassAsync" };

	// sends

	auto intSend{ async(policy, [intKey]() {
		log("Sending int");
		try {
			send(intKey, 101);
		}
		catch (char e[]) {
			// manually catch errors since exceptions only propagate on .get()
			log(e);
			terminate();
		}
		log("Sent int");
	}) };

	auto strSend{ async(policy, [strKey]() {
		log("Sending string");
		try {
			send(strKey, string{ "Ra" });
		} catch (char e[]) {
			// manually catch errors since exceptions only propagate on .get()
			log(e);
			terminate();
		}
		log("Sent string");
	}) };
	
	auto customSend{ async(policy, [customKey]() {
		log("Sending custom class");
		CustomClass custom{ 42, 81 };
		custom.incrementA();
		custom.incrementB();
		try {
			send(customKey, custom);
		}
		catch (char e[]) {
			// manually catch errors since exceptions only propagate on .get()
			log(e);
			terminate();
		}
		log("Sent custom class");
	}) };

	// gets

	auto intGet{ async(policy, [intKey, &intSend]() {
		intSend.get(); // await send success
		log(intKey + " -> " + to_string(get<int>(intKey)));
	}) };

	auto strGet{ async(policy, [strKey, &strSend]() {
		strSend.get(); // await send success
		log(strKey + " -> " + get<string>(strKey));
	}) };

	auto customGet{ async(policy, [customKey, &customSend]() {
		customSend.get(); // await send success
		auto savedCustom{ get<CustomClass>(customKey) };
		savedCustom.incrementA();
		savedCustom.incrementB();
		log("A=" + to_string(savedCustom.getA()) + ", B=" + to_string(savedCustom.getB()));
	}) };

	getchar(); // wait before closing
}
