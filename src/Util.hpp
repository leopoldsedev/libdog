#pragma once

#include <vector>


inline int positive_mod(int i, int n) {
	return ((i % n) + n) % n;
}

template<typename T>
inline void remove_duplicates(std::vector<T> &v) {
    // Source: https://stackoverflow.com/a/1041939/3118787
    sort(v.begin(), v.end());
    v.erase(unique(v.begin(), v.end()), v.end());
}
