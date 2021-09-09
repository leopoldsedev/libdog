#pragma once

#include <string>
#include <vector>


inline int positive_mod(int i, int n) {
	return ((i % n) + n) % n;
}

// Remove all duplicates from a vector
template<typename T>
inline void remove_duplicates(std::vector<T>& v) {
    // Source: https://stackoverflow.com/a/1041939/3118787
    sort(v.begin(), v.end());
    v.erase(unique(v.begin(), v.end()), v.end());
}

// Remove all whitespace from a string
inline void erase_whitespace(std::string& str) {
    // Source: https://stackoverflow.com/a/83538/3118787
    str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());
}
