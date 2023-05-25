#include "domain.h"

#include <stdexcept>

namespace domain {

    Stop::Stop(std::string_view name, const geo::Coordinates& coordinates) :
        NameStop(name), Coordinates(coordinates) {
    }

    Bus::Bus(std::string_view name, const std::vector<Stop*>& stops, bool is_circle) :
        NameBus(name), Stops(stops), CircularRoute(is_circle) {
    }

}