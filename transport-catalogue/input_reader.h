#pragma once

#include "transport_catalogue.h"

#include <iostream>

namespace transport {

	void FillIinTheDatabase(TransportCatalogue& catalogue);

	namespace information {

		void FillStopDistances(std::string& line, TransportCatalogue& catalogue);

		std::tuple<std::string, std::vector<const Stop*>, bool> FillBus(std::string& line, TransportCatalogue& catalogue);
		std::pair <std::string, geo::Coordinates> FillStop(std::string& line);
	}
}