#include <libdog/Area.hpp>

#include <cassert>


namespace libdog {

std::string get_area_name(Area area) {
	switch (area) {
		case Path:
			return "Path";
			break;
		case Kennel:
			return "Kennel";
			break;
		case Finish:
			return "Finish";
			break;
		default:
			assert(false);
			break;
	}
}

}
