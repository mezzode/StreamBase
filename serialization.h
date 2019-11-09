#include <string>
#include "cereal/archives/binary.hpp" // assuming we can use appropriate 3rd party libs
#include "cereal/types/string.hpp"

template<class T>
std::string serialize(T data) {
	std::ostringstream out;
	{
		// not using portable binary archive since client and server on same machine so same endianness
		cereal::BinaryOutputArchive oarchive{ out };
		oarchive(data);
	}
	// writeStr.c_str() returns a const char * so would need to copy it to get a non-const *
	return out.str();
}

template<class T>
T deserialize(const std::string& serializedData) {
	T data;
	{
		std::istringstream in{ serializedData };
		cereal::BinaryInputArchive iarchive{ in };
		iarchive(data);
	}
	// writeStr.c_str() returns a const char * so would need to copy it to get a non-const *
	return data;
}