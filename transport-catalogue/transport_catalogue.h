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
		std::string NameStop;
		std::set<std::string> NameBuses;
		std::unordered_map<std::string, int> Distance;
		geo::Coordinates Coordinates;
	};

	struct Bus {
		std::string NameBus;
		std::vector<std::string> NameStops;
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
		
		void AddBus(Bus& bus);
		void AddStop(Stop& inpute_data);

		const Bus* SearchRoute(const std::string_view name) const;
		const Stop* SearchStop(const std::string_view name) const;
		Stop* SearchStopSetDistance(const std::string_view name) const;

		const RouteInformation GetRouteInformation(const std::string& name) const;

		const std::set<std::string> GetStopRoutes(std::string_view name) const;

		void SetDistanceBetweenStops( Stop* from, Stop* to, int distance);
		int GetDistanceBetweenStops(const Stop* from, const Stop* to) const;

	private:

		std::unordered_map<std::string_view, const Bus*> BusNameToBus;
		std::unordered_map<std::string_view, Stop*> StopNameToStop;

		std::deque<Stop> AllStops;
		std::deque<Bus> AllBuses;

	};


}