#include <string>

template<class T> std::string serialize(T data);
template<class T> T deserialize(const std::string& serializedData);
