#pragma once

#include <iostream>

#define HERE (std::cout << "HERE! " << __FILE__ << ":" << __LINE__ << std::endl)

#define PRINT_DBG(x) (std::cout << #x << ":" << std::endl << (x) << std::endl)
