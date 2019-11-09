constexpr LPCSTR pipeName{ R"(\\.\pipe\StreamBase)" };
constexpr DWORD bufSize{ 512 };

enum Type {
	Send,
	Get
};

struct Action {
	Type type;
	string key;
};
