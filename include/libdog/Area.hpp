#pragma once

#include <string>
#include <cassert>


namespace libdog {

enum Area {
	Path,
	Kennel,
	Finish
};

std::string get_area_name(Area area);

}
