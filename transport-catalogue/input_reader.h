#pragma once

#include "transport_catalogue.h"

#include <iostream>

namespace transport {

	void FillIinTheDatabase(TransportCatalogue& catalogue);

	namespace information {

		void FillStopDistances(std::string& line, TransportCatalogue& catalogue);

		Bus FillBus(std::string& line);
		Stop FillStop(std::string& line);
	}
}