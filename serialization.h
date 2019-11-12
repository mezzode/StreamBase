#pragma once

#include <string>
#include "cereal/archives/binary.hpp"
#include "cereal/types/string.hpp"

template<class T>
std::string serialize(T data) {
	std::ostringstream out;
	{
		// not using portable binary archive since client and server on same machine so same endianness
		cereal::BinaryOutputArchive oarchive{ out };
		oarchive(data);
	}
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
	return data;
}