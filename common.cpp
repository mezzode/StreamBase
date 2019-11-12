#include <iostream>
#include <mutex>
#include "common.h"

std::mutex coutLock;

void log(std::string msg) {
	std::lock_guard<std::mutex> lg{ coutLock };
	std::cout << msg << std::endl;
}