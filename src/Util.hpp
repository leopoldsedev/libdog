#pragma once


inline int positive_mod(int i, int n) {
	return ((i % n) + n) % n;
}
