#pragma once

class CustomClass {
public:
	int counterA;

	CustomClass(int a = 0, int b = 0);
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

template<class Archive>
void serialize(Archive& archive, CustomClass& m) {
	archive(m.counterA, m.counterB);
}
