#include "CustomClass.h"

CustomClass::CustomClass(int a, int b) : counterA{ a }, counterB{ b } {}

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
