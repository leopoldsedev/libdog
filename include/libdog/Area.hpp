#pragma once

#include <string>


namespace libdog {

enum Area {
	Path,
	Kennel,
	Finish
};

std::string get_area_name(Area area);

}
