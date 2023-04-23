#pragma once

#include "transport_catalogue.h"

#include <iostream>

namespace transport {

	void GetInformation(const TransportCatalogue& catalogue);

	namespace information {

		void PrintRoute(std::string& line, const TransportCatalogue& catalogue);
		void PrintStop(std::string& line, const TransportCatalogue& catalogue);

	}
}