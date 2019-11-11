#pragma once

class CustomClass {
public:
	int counterA;

	CustomClass(int a, int b);
	void incrementA();
	void incrementB();
	int getA();
	int getB();
private:
	int counterB;

	template<class Archive>
	friend void serialize(Archive& archive, CustomClass& m);
	// external serialize function must be friend to serialize private attribute
};