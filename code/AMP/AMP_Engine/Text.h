#pragma once

#include "functional"
#include "string"


/// Helper struct for high performance string comparison
/// 
/// This is a wrapper around std::string for comparing and finding strings
/// @note The idea is to calculate a hash when the string is built and compare from this point on just hashes
struct TEXT
{
private:
	std::string txt;
	size_t hash = 0;
public:
	/// TEXT constructor
	/// 
	/// @param AssetPath uses a std::string as input to calculate hash
	TEXT(std::string AssetPath):txt(AssetPath) {
		std::hash<std::string> hash_fn;
		hash = hash_fn(AssetPath);
	}
	TEXT() {};

	/// @return char* most useful for C-libraries
	const char* getCharArray() {
		return txt.c_str();
	}

	/// @return the stored string
	std::string getString() {
		return txt;
	}

	/// @return the hash which was calculated inside the constructor
	size_t getHash() {
		return hash;
	}

	/// @return the length of the actual std::string
	size_t getTextLength() {
		return txt.size();
	}

	/// hash comparison
	bool operator==(TEXT &rhs) {
		return this->hash == rhs.hash;
	}

	/// hash comparison (makes no sense in terms of content of the string)
	bool operator>(TEXT& rhs) {
		return this->hash > rhs.hash;
	}

	/// @return character a certain string position
	char operator[](int element) {
		return this->txt[element];
	}
};
