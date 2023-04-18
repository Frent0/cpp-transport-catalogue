#pragma once

#include "geo.h"

#include <iostream>
#include <deque>
#include <set>
#include <vector>
#include <string_view>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <unordered_set>

namespace transport {

	struct Stop {
		std::string Name_Stop;
		std::set<std::string> Name_Buses;
		std::unordered_map<std::string, int> distance;
		geo::Coordinates coordinates;
	};

	struct Bus {
		std::string Name_Bus;
		std::vector<std::string> Name_Stops;
		bool CircularRoute;
	};

	struct RouteInformation {
		std::string NameRoute;
		int StopCount;
		int UniqueStopCount;
		double RouteLength;
		double Curvature;
	};

	class TransportCatalogue {
	public:

		void AddBus(Bus&& bus);
		void AddStop(Stop&& inpute_data);

		const Bus* SearchRoute(const std::string& name) const;
		Stop* SearchStop(const std::string& name) const;

		const RouteInformation GettingRouteInformation(const std::string& name) const;

		const std::set<std::string> GettingStopRoutes(std::string name) const;

		void SettingDistanceBetweenStops(Stop* from, Stop* to, int distance);
		int GettingDistanceBetweenStops(Stop* from, Stop* to) const;

	private:

		std::unordered_map<std::string_view, const Bus*> BusName_To_Bus;
		std::unordered_map<std::string_view, Stop*> StopName_To_Stop;

		std::deque<Stop> all_stops;
		std::deque<Bus> all_bus;

	};
}