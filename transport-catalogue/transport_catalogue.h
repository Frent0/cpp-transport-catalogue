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

	struct Stop;

	struct Bus {
		Bus(const std::string& name, const std::vector<Stop*>& stops, bool is_circle) :
			NameBus(name), Stops(stops), CircularRoute(is_circle) {

		}

		std::string NameBus;
		std::vector<Stop*> Stops;
		bool CircularRoute;

		struct cmp_ptr
		{
			bool operator()(const Bus* lhs, const Bus* rhs) const
			{
				return lhs->NameBus < rhs->NameBus;
			}
		};
	};


	struct Stop {

		Stop(const std::string& name, const geo::Coordinates& coordinates) :
			NameStop(name), Coordinates(coordinates) {

		}

		std::string NameStop;
		std::set < Bus*, Bus::cmp_ptr> NameBuses;
		std::unordered_map<Stop*, int> Distance;
		geo::Coordinates Coordinates;
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

		void AddBus(const std::string& num, const std::vector<Stop*>& stops, bool is_circle);
		void AddStop(const std::string& name, const geo::Coordinates& coordinates);

		const Bus* SearchRoute(std::string_view name) const;
		const Stop* SearchStop(std::string_view name) const;

		Stop* SearchStopSetDistance(std::string_view name) const;

		const RouteInformation GetRouteInformation(std::string_view name) const;

		const std::set<Bus*, Bus::cmp_ptr> GetStopRoutes(std::string_view name) const;

		void SetDistanceBetweenStops(Stop* from, Stop* to, int distance);
		int GetDistanceBetweenStops(Stop* from, Stop* to) const;

	private:

		std::unordered_map<std::string_view, Bus*> BusNameToBus;
		std::unordered_map<std::string_view, Stop*> StopNameToStop;

		std::deque<Stop> AllStops;
		std::deque<Bus> AllBuses;

	};


}