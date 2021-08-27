#pragma once


int positive_mod(int i, int n) {
	return ((i % n) + n) % n;
}
