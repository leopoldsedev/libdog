#pragma once

#include <string>


namespace libdog {

enum Area {
	Path = 1,
	Kennel = 0,
	Finish = 2
};

std::string get_area_name(Area area);

}
