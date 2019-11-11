#pragma once

#include <string>

constexpr LPCSTR pipeName{ R"(\\.\pipe\StreamBase)" };
constexpr DWORD bufSize{ 512 };

enum class Type {
	Send,
	Get
};

struct Action {
	Type type;
	std::string key;

	template <class Archive>
	void serialize(Archive& archive) {
		archive(type, key);
	}
};

const std::string saveSuccessStatus{ "saveSuccess" };
