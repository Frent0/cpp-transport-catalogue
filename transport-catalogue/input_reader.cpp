#include "input_reader.h"

namespace transport {

	void FillIinTheDatabase(TransportCatalogue& catalogue) {
		size_t count_fill;

		std::vector<std::string> buses;
		std::vector<std::string> stops;
		std::vector<std::string> distance;

		std::cin >> count_fill;

		for (size_t count = 0; count < count_fill; ++count) {
			std::string name_request, line;

			std::cin >> name_request;
			std::getline(std::cin, line);

			if (name_request == "Stop") {
				stops.push_back(line);
			}
			if (name_request == "Bus") {
				buses.push_back(line);
			}
		}

		distance = stops;

		for (std::string& stop : stops) {
			auto stop_result = information::FillStop(stop);
			catalogue.AddStop(stop_result.first, stop_result.second);
		}

		for (std::string& distance : distance) {
			information::FillStopDistances(distance, catalogue);
		}

		for (std::string& bus : buses) {
			auto bus_result = information::FillBus(bus, catalogue);
			catalogue.AddBus(std::get<0>(bus_result), std::get<1>(bus_result), std::get<2>(bus_result));
		}

	}

	namespace information {

		std::tuple<std::string, const std::vector<Stop*>, bool> FillBus(std::string& line, TransportCatalogue& catalogue) {
			std::vector<Stop*> stops_name;

			std::string name_bus = line.substr(1, line.find_first_of(':') - 1);
			line.erase(0, line.find_first_of(':') + 2);

			char pos = line.find_first_of('-') != line.npos ? '-' : '>';
			bool circular = pos == '>';

			while (line.find_first_of(pos) != line.npos) {
				stops_name.push_back(catalogue.SearchStopSetDistance(line.substr(0, line.find_first_of(pos) - 1)));
				line.erase(0, line.find_first_of(pos) + 2);
			}

			stops_name.push_back(catalogue.SearchStopSetDistance(line.substr(0, line.npos - 1)));

			return { name_bus,stops_name,circular };

		}

		std::pair <std::string, geo::Coordinates> FillStop(std::string& line) {

			std::string name_stop = line.substr(1, line.find_first_of(':') - 1);

			double latitude = std::stod(line.substr(line.find_first_of(':') + 2, 9));
			double longitude;

			line.erase(0, line.find_first_of(',') + 2);

			if (line.find_last_of(',') == line.npos) {
				longitude = std::stod(line.substr(0, line.npos - 1));
				line.clear();
			}
			else {
				longitude = std::stod(line.substr(0, line.find_first_of(',')));
				line.erase(0, line.find_first_of(',') + 2);
			}

			geo::Coordinates coordinates{ latitude,longitude };
			return{ name_stop,coordinates };
		}

		void FillStopDistances(std::string& line, TransportCatalogue& catalogue) {
			if (!line.empty()) {

				std::string stop_from_name = FillStop(line).first;
				Stop* from = catalogue.SearchStopSetDistance(stop_from_name);

				while (!line.empty()) {
					int distance = 0;
					std::string stop_to_name;

					distance = std::stoi(line.substr(0, line.find_first_of("m to ")));
					line.erase(0, line.find_first_of("m to ") + 5);

					if (line.find("m to ") == line.npos) {

						stop_to_name = line.substr(0, line.npos - 1);
						Stop* to = catalogue.SearchStopSetDistance(stop_to_name);

						catalogue.SetDistanceBetweenStops(from, to, distance);

						if (!catalogue.SearchStop(to->NameStop)->Distance.count(from)) {
							catalogue.SetDistanceBetweenStops(to, from, distance);
						}

						line.clear();
					}
					else {
						stop_to_name = line.substr(0, line.find_first_of(','));
						Stop* to = catalogue.SearchStopSetDistance(stop_to_name);

						catalogue.SetDistanceBetweenStops(from, to, distance);

						if (!catalogue.SearchStop(to->NameStop)->Distance.count(from)) {
							catalogue.SetDistanceBetweenStops(to, from, distance);
						}

						line.erase(0, line.find_first_of(',') + 2);
					}
				}
			}

		}
	}
}