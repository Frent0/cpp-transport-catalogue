#pragma once

#include "transport_catalogue.h"

#include <iostream>

namespace transport {

	void GettingInformation(TransportCatalogue& catalogue);

	namespace information {

		void PrintRoute(std::string& line, TransportCatalogue& catalogue);
		void PrintStop(std::string& line, TransportCatalogue& catalogue);

	}
}